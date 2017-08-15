#include "myvi.h"
#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>

extern int cursor_freq;
int pre_cursor_freq;

FILE file;
int is_new_file;

char buffer[BUFFER_SIZE];
char instruction[COLUMN_LEN] = "";
char *filename;
int inst_len = 0;
int size = 0;
int cursor_location;
int page_location = 0;
int page_end;
int err;
int mode;

char myvi_init() {
    int i;
    size = 0;
    inst_len = 0;
    cursor_location = 0;
    page_location = 0;
    err = 0;
    mode = 0;
    page_end = 0;
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
    return 0;
}

char to_lower_case(char ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 'a';
    else
        return ch;
}

char *mystrcpy(char *dest, const char *src) {
    do {
        *(dest++) = *(src++);
    } while (*src);

    return dest;
}

void load_file(char *file_path) {
    int file_size;
    int cnt = 0;
    unsigned char newch;
    unsigned int ret = fs_open(&file, file_path);

    if (ret != 0) {
        is_new_file = 1;
        buffer[size++] = '\n';
        return;
    } else {
        is_new_file = 0;
    }

    file_size = get_entry_filesize(file.entry.data);
    int i = 0;
    for (i = 0; i < file_size; i++) {
        fs_read(&file, &newch, 1);
        if (newch != 13) {
            buffer[size++] = (char)newch;
        }
        if (size == BUFFER_SIZE - 1) {
            err = 2;
            return;
        }
    }

    if (size == 0 || buffer[size - 1] != '\n') {
        buffer[size++] = '\n';
    }
    fs_close(&file);
}

void save_file() {
    if (is_new_file) {
        fs_create(filename);
    }

    fs_open(&file, filename);
    fs_lseek(&file, 0);
    fs_write(&file, buffer, size);
    int ret = fs_close(&file);
}

void insert_key(char key, int site) {
    if (size >= BUFFER_SIZE) {
        err = 1;
        return;
    }

    int i = 0;
    for (i = size; i > site; i--)
        buffer[i] = buffer[i - 1];
    buffer[site] = key;
    size++;
}

void delete_key(int site) {
    int i = 0;
    for (i = site; i < size - 1; i++)
        buffer[i] = buffer[i + 1];
    size--;
}

void put_char_on_screen(char ch, int row, int column, int color) {
    kernel_putchar_at(ch, color & 0xFFFF, (color >> 16) & 0xFFFF, row, column);
}

void screen_flush() {
    int row = 0, column = 0;
    int loc = page_location;
    int next_column, color;

    while (row < ROW_LEN && loc < size) {
        if (loc != cursor_location)
            color = COLOR_BLACK_WHITE;
        else
            color = COLOR_WHITE_BLACK;

        switch (buffer[loc]) {
            case KEYBOARD_ENTER_N:
                put_char_on_screen(KEYBOARD_SPACE, row, column, color);
                column++;
                color = COLOR_BLACK_WHITE;
                for (; column < COLUMN_LEN; column++)
                    put_char_on_screen(KEYBOARD_SPACE, row, column, color);
                row++;
                column = 0;
                break;
            case KEYBOARD_ENTER_R:
                break;
            case KEYBOARD_TAB:
                next_column = (column & 0xFFFFFFFC) + 4;
                for (; column < next_column; column++)
                    put_char_on_screen(KEYBOARD_SPACE, row, column, color);
                break;
            default:  // other ascii character
                put_char_on_screen(buffer[loc], row, column, color);
                column++;
        }

        if (column == COLUMN_LEN) {
            row++;
            column = 0;
        }

        loc++;
    }

    page_end = loc;

    if (loc == size && loc == cursor_location) {
        put_char_on_screen(KEYBOARD_SPACE, row, column, COLOR_WHITE_BLACK);
        column++;
        if (column == COLUMN_LEN) {
            row++;
            column = 0;
        }
    }

    if (row < ROW_LEN) {
        if (column != 0) {
            for (; column < COLUMN_LEN; column++)
                put_char_on_screen(KEYBOARD_SPACE, row, column, COLOR_BLACK_WHITE);
            row++;
            column = 0;
        }
        for (; row < ROW_LEN; row++) {
            put_char_on_screen('~', row, column, COLOR_BLACK_BLUE);
            column++;
            for (; column < COLUMN_LEN; column++)
                put_char_on_screen(KEYBOARD_SPACE, row, column, COLOR_BLACK_WHITE);
            column = 0;
        }
    }

    int i = 0;
    for (i = 0; i < COLUMN_LEN - 20; i++) {
        if (i < inst_len)
            put_char_on_screen(instruction[i], ROW_LEN, i, COLOR_GREEN_WHITE);
        else
            put_char_on_screen(' ', ROW_LEN, i, COLOR_GREEN_WHITE);
    }
}

char get_key() {
    return kernel_getchar();
}

void page_location_last_line() {
    int loc = page_location;
    do {
        loc--;
    } while (loc > 0 && buffer[loc - 1] != '\n' && buffer[loc - 1] != '\r');
    if (loc >= 0)
        page_location = loc;
}

void page_location_next_line() {
    int loc = page_location;
    while (loc < size && buffer[loc] != '\n')
        loc++;
    if (loc + 1 < size)
        page_location = loc + 1;
}

void cursor_prev_line() {
    int loc = cursor_location;
    int offset = 0;
    do {
        loc--;
        offset++;
    } while (loc > 0 && buffer[loc - 1] != '\n');

    if (loc <= 0)
        return;
    while (loc > 0 && buffer[loc - 1] != '\n')
        loc--;
    cursor_location = loc;
}

void cursor_next_line() {
    int loc = cursor_location;
    while (loc < size && buffer[loc] != '\n')
        loc++;
    loc++;
    if (loc < size)
        cursor_location = loc;
}

void do_command_mode(char key) {
    switch (key) {
        case 'j':
            cursor_next_line();
            break;
        case 'h':
            if (cursor_location > 0 && buffer[cursor_location - 1] != '\n')
                cursor_location--;
            break;
        case 'k':
            cursor_prev_line();
            break;
        case 'l':
            if (cursor_location + 1 < size && buffer[cursor_location] != '\n')
                cursor_location++;
            break;
        case 'x':
            if (cursor_location != size - 1)
                delete_key(cursor_location);
            break;
        case ':':
            mode = 2;
            instruction[0] = ':';
            int i = 0;
            for (i = 1; i < COLUMN_LEN; i++)
                instruction[i] = ' ';
            inst_len = 1;
            break;
        case 'i':
            mode = 1;
            return;
        default:
            break;
    }
    if (cursor_location < page_location)
        page_location_last_line();
    else if (cursor_location >= page_end)
        page_location_next_line();
    screen_flush();
}

void do_insert_mode(char key) {
    switch (key) {
        case 27:
            // case 'q':
            mode = 0;
            return;
        case 0x8:
            if (cursor_location != 0)
                delete_key(cursor_location - 1);
            cursor_location--;
            screen_flush();
            if (cursor_location < page_location)
                page_location_last_line();
            break;
        default:
            insert_key(key, cursor_location);
            cursor_location++;
            screen_flush();  // this line is needed because page_end may changed
                             // after insertion
            if (cursor_location >= page_end)
                page_location_next_line();
            break;
    }
    screen_flush();
}

void do_last_line_mode(char key) {
    switch (key) {
        case 27:  // ESC
            inst_len = 0;
            mode = 0;
            break;
        case 8:
            if (inst_len > 0)
                inst_len--;
            break;
        case '\n':
            if (inst_len > 0 && instruction[0] == ':') {
                if (inst_len == 3 && instruction[1] == 'q' && instruction[2] == '!') {
                    err = 1;
                } else if (inst_len == 3 && instruction[1] == 'w' && instruction[2] == 'q') {
                    save_file();
                    err = 1;
                }
            }
            break;
        default:
            instruction[inst_len++] = to_lower_case(key);
            break;
    }
    screen_flush();
}

int myvi(char *para) {
    myvi_init();

    filename = para;
    pre_cursor_freq = cursor_freq;
    cursor_freq = 0;
    kernel_set_cursor();

    mystrcpy(file.path, filename);

    load_file(filename);

    screen_flush();

    /* global variable initial */

    while (err == 0) {
        char key = get_key();
        switch (mode) {
            case 0:  // command mode
                do_command_mode(key);
                break;
            case 1:  // insert mode
                do_insert_mode(key);
                break;
            case 2:  // last line mode
                do_last_line_mode(key);
                break;
            default:
                break;
        }
    }

    cursor_freq = pre_cursor_freq;
    kernel_set_cursor();
    kernel_clear_screen(31);

    return 0;
}