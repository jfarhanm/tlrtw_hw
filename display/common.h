#ifndef DISPLAY_COMMON_H
#define DISPLAY_COMMON_H
#include <stdint.h>
int oled_print_text(char* text);
void oled_print_character(int row, int column, uint8_t character);
void oled_init(int rows, int cols);
void oled_clear();
void debug_logger_flush_one();
int debug_logger_log(char* text);
void debug_logger_init();
void debug_logger_screen_clear();
#endif
