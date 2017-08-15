#ifndef _MYVI_H
#define _MYVI_H

//#define debug

#define BUFFER_SIZE 4096
#define ROW_LEN 29
#define COLUMN_LEN 80

#define KEYBOARD_SPACE 32
#define KEYBOARD_TAB 9
#define KEYBOARD_ENTER_N 10
#define KEYBOARD_ENTER_R 13

#define COLOR_BLACK_WHITE 0x00000FFF
#define COLOR_WHITE_BLACK 0x0FFF0000
#define COLOR_BLACK_BLUE 0x0000000F
#define COLOR_GREEN_WHITE 0x00F00FFF

extern char buffer[BUFFER_SIZE];
extern char instruction[COLUMN_LEN];
extern int size;
extern int cursor_location;
extern int page_location;
extern int page_end;
extern int err;
extern int mode;

void load_file(char* file_path);
void insert_key(char key, int site);
void delete_key(int site);
void put_char_on_screen(char ch, int row, int column, int color);
void screen_flush();
char get_key();
void page_location_last_line();
void page_location_next_line();
void cursor_prev_line();
void cursor_next_line();
void do_command_mode(char key);
void do_insert_mode(char key);
void do_last_line_mode(char key);
int myvi(char* filename);

#endif  // _MYVI_H
