#include "ps2.h"
#include <driver/vga.h>
#include <intr.h>

#pragma GCC push_options
#pragma GCC optimize("O0")

static unsigned int* const PS2_PHY = (unsigned int*)0xbfc09010;

static volatile unsigned int keyboard_state = 0;
#define CAPSLOCK_MASK 4
#define NUMLOCK_MASK 2
#define SCRLOCK_MASK 1
#define SHIFT_MASK 8
#define CTRL_MASK 16
#define ALT_MASK 32

static unsigned int buffer[32];
static unsigned int ready[32];
static volatile int buffer_wptr = 0;
static volatile int buffer_rptr = 0;
static unsigned int key_buffer = 0;
static unsigned int keyboard_cmd_state = 0;

signed char scantoascii_uppercase[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x09, 0x7E, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x51,
    0x21, 0xff, 0xff, 0xff, 0x5a, 0x53, 0x41, 0x57, 0x40, 0xff, 0xff, 0x43, 0x58, 0x44, 0x45, 0x24, 0x23, 0xff, 0xff, 0x20, 0x56, 0x46,
    0x54, 0x52, 0x25, 0xff, 0xff, 0x4e, 0x42, 0x48, 0x47, 0x59, 0x5E, 0xff, 0xff, 0xff, 0x4d, 0x4a, 0x55, 0x26, 0x2A, 0xff, 0xff, 0x3c,
    0x4b, 0x49, 0x4f, 0x29, 0x28, 0xff, 0xff, 0x3E, 0x3f, 0x4c, 0x3A, 0x50, 0x5F, 0xff, 0xff, 0xff, 0x22, 0xff, 0x7B, 0x2B, 0xff, 0xff,
    0xff, 0xff, 0x0a, 0x7D, 0xff, 0x7C, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1B, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
signed char scantoascii_lowercase[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x09, 0x60, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x71,
    0x31, 0xff, 0xff, 0xff, 0x7a, 0x73, 0x61, 0x77, 0x32, 0xff, 0xff, 0x63, 0x78, 0x64, 0x65, 0x34, 0x33, 0xff, 0xff, 0x20, 0x76, 0x66,
    0x74, 0x72, 0x35, 0xff, 0xff, 0x6e, 0x62, 0x68, 0x67, 0x79, 0x36, 0xff, 0xff, 0xff, 0x6d, 0x6a, 0x75, 0x37, 0x38, 0xff, 0xff, 0x2c,
    0x6b, 0x69, 0x6f, 0x30, 0x39, 0xff, 0xff, 0x2e, 0x2f, 0x6c, 0x3b, 0x70, 0x2d, 0xff, 0xff, 0xff, 0x27, 0xff, 0x5b, 0x3d, 0xff, 0xff,
    0xff, 0xff, 0x0a, 0x5d, 0xff, 0x5c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1B, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#ifdef PS2_DEBUG
void print_wptr() {
    int row, col;
    row = cursor_row;
    col = cursor_col;
    cursor_row = 19;
    cursor_col = 32;
    kernel_printf("Wptr: %x\n", buffer_wptr);
    cursor_row = row;
    cursor_col = col;
}

void print_rptr() {
    int row, col;
    row = cursor_row;
    col = cursor_col;
    cursor_row = 20;
    cursor_col = 32;
    kernel_printf("Rptr: %x\n", buffer_rptr);
    cursor_row = row;
    cursor_col = col;
}

void print_buffer() {
    int row, col;
    int i;
    row = cursor_row;
    col = cursor_col;
    cursor_row = 21;
    cursor_col = 32;
    for (i = 0; i < 32; i++) {
        kernel_printf("%x ", ready[i]);
        if (7 == (i % 8)) {
            cursor_col = 32;
            cursor_row++;
        }
    }
    for (i = 0; i < 32; i++) {
        kernel_printf("%x ", buffer[i]);
        if (7 == (i % 8)) {
            cursor_col = 32;
            cursor_row++;
        }
    }
    cursor_row = row;
    cursor_col = col;
}

void print_curr_key(int key) {
    int row, col;
    row = cursor_row;
    col = cursor_col;
    cursor_row = 18;
    cursor_col = 32;
    kernel_printf("Key: %x\n", key);
    cursor_row = row;
    cursor_col = col;
}

void print_curr_char(int key) {
    int row, col;
    row = cursor_row;
    col = cursor_col;
    cursor_row = 17;
    cursor_col = 32;
    kernel_printf("Char: %c\n", key);
    cursor_row = row;
    cursor_col = col;
}

#endif  // ! PS2_DEBUG

void init_buffer() {
    int i = 0;
    for (i = 0; i < 32; i++) {
        buffer[i] = 0;
        ready[i] = 0;
    }
    buffer_wptr = 0;
    buffer_rptr = 0;
}

void init_ps2() {
    init_buffer();
    register_interrupt_handler(2, ps2_handler);
    PS2_PHY[1] = -1;  // Enable ps/2 interrupt
}

void ps2_handler(unsigned int status, unsigned int cause, context* pt_context) {
    unsigned int ps2_ctrl_reg;
    unsigned int ps2_data_reg;
    ps2_ctrl_reg = PS2_PHY[1];
    if ((ps2_ctrl_reg >> 16) & 7)
        return;
    while ((ps2_ctrl_reg & 0x3f) > 0) {
        ps2_data_reg = PS2_PHY[0];
        if (ps2_data_reg == 0xfa)  // keyboard ACK
        {
            switch (keyboard_cmd_state) {
                case 1:
                    PS2_PHY[0] = (keyboard_state & 7);  // Set LED state
                    break;
            }
            keyboard_cmd_state = 0;
            continue;
        }
        key_buffer = (key_buffer << 8) | ps2_data_reg;
        if (ps2_data_reg < 0x80) {
            if ((key_buffer & 0x7f) == key_buffer) {
                buffer[buffer_wptr] = key_buffer;
                ready[buffer_wptr] = 1;
                buffer_wptr = (buffer_wptr + 1) & 31;
#ifdef PS2_DEBUG
                print_wptr();
#endif  // ! PS2_DEBUG
            }
#ifdef PS2_DEBUG
            print_buffer();
#endif  // ! PS2_DEBUG
            switch (key_buffer) {
                case 0x58:  // Caps lock down
                    keyboard_state ^= CAPSLOCK_MASK;
                    keyboard_cmd_state = 1;
                    PS2_PHY[0] = 0xed;
                    break;
                case 0x12:  // LShift down
                case 0x59:  // RShift down
                    keyboard_state |= SHIFT_MASK;
                    break;
                case 0xf012:  // LShift up
                case 0xf059:  // RShift up
                    keyboard_state &= ~SHIFT_MASK;
                    break;
                case 0x14:    // LCtrl down
                case 0xe014:  // RCtrl down
                    keyboard_state |= CTRL_MASK;
                    break;
                case 0xf014:    // LCtrl up
                case 0xe0f014:  // RCtrl up
                    keyboard_state &= ~CTRL_MASK;
                    break;
                case 0x11:    // LAlt down
                case 0xe011:  // RAlt down
                    keyboard_state |= ALT_MASK;
                    break;
                case 0xf011:    // LAlt up
                case 0xe0f011:  // RAlt up
                    keyboard_state &= ~ALT_MASK;
                    break;
            }
            key_buffer = 0;
        }

        ps2_ctrl_reg = PS2_PHY[1];
    }
}

int kernel_getkey() {
    int temp;
    if (ready[buffer_rptr] == 0) {
        return 0xfff;
    }
    temp = buffer[buffer_rptr];
#ifdef PS2_DEBUG
    print_curr_key(temp);
#endif  // ! PS2_DEBUG
    ready[buffer_rptr] = 0;
    buffer_rptr = buffer_rptr + 1;
    if (buffer_rptr == 32) {
        init_buffer();
    }
#ifdef PS2_DEBUG
    print_rptr();
#endif  // ! PS2_DEBUG
    return temp;
}

int kernel_scantoascii(int key) {
    int temp;
    if ((key & 0x7f) != key)
        return -1;
    if (!(keyboard_state & CAPSLOCK_MASK) != !(keyboard_state & SHIFT_MASK))
        return (int)scantoascii_uppercase[key];
    else
        return (int)scantoascii_lowercase[key];
}

void sleep(int cycle) {
    while (cycle--)
        ;
}

int kernel_getchar() {
    int key;
    do {
        key = kernel_scantoascii(kernel_getkey());
        sleep(1000);
    } while (key == -1);
#ifdef PS2_DEBUG
    print_curr_char(key);
#endif  // ! PS2_DEBUG
    return key;
}

#pragma GCC pop_optimize