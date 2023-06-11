#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "modded_fonts.h"
#include "common.h"
// NOTE MOVE TO MAIN HEADER FILE
#define OLED_I2C_SCL_PIN 4
#define OLED_I2C_SDA_PIN 5
#define OLED_I2C_I2C_CTRL i2c1
// NOTE: CHANGE I2C DEFAULT TO SOME OTHER I2C



// HARDWARE FUNCTIONS
#define OLED_DISPLAY_WIDTH _u(128)
#define OLED_DISPLAY_NUM_PAGES _u(8)
#define OLED_DISPLAY_HEIGHT _u(64)

#define OLED_SET_PAGE_ADDR _u(0xB0)
#define OLED_SET_LOW_COL_ADDR _u(0x00)
#define OLED_SET_HIGH_COL_ADDR _u(0x10)
#define OLED_SET_DISP _u(0xAE)

#define OLED_ADDR _u(0x3C)


void oled_hw_send_cmd(uint8_t cmd){
	uint8_t buf[2] = {0x80,cmd};
	i2c_write_blocking(i2c_default,(OLED_ADDR&0xFE),buf,2,false);
}

void oled_hw_send_data(uint8_t *buf,int size){
	i2c_write_blocking(i2c_default,(OLED_ADDR&0xFE),buf,size,false);
}

void oled_hw_init(){
	bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
	i2c_init(i2c_default, 400 * 1000);
	gpio_set_function(OLED_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(OLED_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
	oled_hw_send_cmd(OLED_SET_DISP|0x01);
	sleep_ms(2000);
	oled_hw_send_cmd(OLED_SET_DISP|0x01);
	sleep_ms(2000);
}


static uint8_t* oled_hw_render_temp_buf;
volatile static uint8_t is_oled_temp_initialised;
void oled_hw_clear_page_buf(){
	if(!is_oled_temp_initialised){
		oled_hw_render_temp_buf= malloc(OLED_DISPLAY_WIDTH + 1);
	}
	for(int i=0;i<OLED_DISPLAY_WIDTH + 1;i++)
		oled_hw_render_temp_buf[i]=0;
}


void oled_hw_send_page(uint8_t* buf,int size,uint8_t page){
	oled_hw_send_cmd(OLED_SET_PAGE_ADDR|page);
	oled_hw_send_cmd(OLED_SET_LOW_COL_ADDR|2);
	oled_hw_send_cmd(OLED_SET_HIGH_COL_ADDR|0);
	oled_hw_send_cmd(0xE0);
	oled_hw_send_data(buf,size);
	oled_hw_send_cmd(0xEE);
}



void oled_hw_render(uint8_t* buf){
	oled_hw_clear_page_buf();
	oled_hw_render_temp_buf[0] = 0x40;
	for (int page=0;page<OLED_DISPLAY_NUM_PAGES;page++){
		for(int j=1;j<OLED_DISPLAY_WIDTH+1;j++){
			oled_hw_render_temp_buf[j] = buf[page*OLED_DISPLAY_WIDTH + j];
		}
		oled_hw_send_page(oled_hw_render_temp_buf,OLED_DISPLAY_WIDTH,page);
	}
}
// END OF HARDWARE FUNCTIONS



// SOFTWARE FUNCTIONS
typedef struct OLEDController{
	uint8_t* databuffer;
	int DATABUFFER_SIZE;
	uint8_t** FONT_LIST;
	uint16_t DISPLAY_NUM_ROWS;
	uint16_t DISPLAY_NUM_COLS;
	

	uint16_t TEXT_NUM_ROWS;
	uint16_t TEXT_NUM_COLS;

	int text_current_row;
	int text_current_col;
}OLEDController;

volatile static OLEDController* oled_controller=NULL;

void oled_clear(){
	if(oled_controller==NULL)
		return;

	for(int i=0; i<oled_controller->DATABUFFER_SIZE	;i++){
		oled_controller->databuffer[i]=0;
	}	
	oled_controller->text_current_row=0;
	oled_controller->text_current_col=0;
	oled_hw_render(oled_controller->databuffer);
}


void oled_print_character(int row, int column, uint8_t character){
	if(oled_controller==NULL)
		return;
	int block = column;
	int page = row;
	for (int i=0;i<8;i++){
		oled_controller->databuffer[OLED_DISPLAY_WIDTH*page + 8*block + i] 
			=fonts[(int)character][i];//oled_controller->FONT_LIST[(int)character][i];
	}
}

// If Ok sends zero
// If Error sends number of characters printed
#define CHARS_PRNT_OK 0
int oled_print_text(char* text){
	for(int i=0;text[i]!='\0';i++){
		oled_controller->text_current_col = oled_controller->text_current_col + 1;
		if(oled_controller->text_current_col==oled_controller->TEXT_NUM_COLS||text[i]=='\n'||text[i]=='\0'){
			oled_controller->text_current_row++;
			if(oled_controller->text_current_row >=oled_controller->TEXT_NUM_ROWS){
				oled_print_character((oled_controller->text_current_row)-1,oled_controller->text_current_col,text[i]);
				oled_controller->text_current_row=0;
				oled_hw_render(oled_controller->databuffer);
				return i;
			}
			oled_controller->text_current_col=0;
		}
		oled_print_character(oled_controller->text_current_row,oled_controller->text_current_col,text[i]);
	}	
	oled_hw_render(oled_controller->databuffer);
	// A minimum of a 30ms delay feels safe.
	sleep_ms(30);
	return CHARS_PRNT_OK;
}


// ROWS AND COLUMNS OF THE OLED ARRAY
void oled_init(int rows, int cols){
	if(oled_controller!=NULL)
	{
		return;
	}

	oled_controller = malloc(sizeof(OLEDController));
	oled_controller->databuffer = malloc(rows*cols);
	oled_controller->FONT_LIST = fonts;
	oled_controller->DISPLAY_NUM_ROWS = (uint16_t)rows;
	oled_controller->DISPLAY_NUM_COLS = (uint16_t)cols;
	oled_controller->DATABUFFER_SIZE = rows*cols;
	
	// selecting 8x8 font by default
	oled_controller->TEXT_NUM_ROWS = (uint16_t)(rows/8);
	oled_controller->TEXT_NUM_COLS = (uint16_t)(cols/8);
	oled_controller->text_current_row=0;
	oled_controller->text_current_col=0;

	oled_hw_init();
	oled_clear();
}


//DEBUG FUNCTIONS
void debug_print_databuffer(uint8_t* buf, int width,int height){
	
}
