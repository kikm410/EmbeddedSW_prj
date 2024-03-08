//fnd.c

#include "numbaseball.h"

extern payload *gameStatus;

const unsigned char fpga_dot_digit[][10] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // Blank
    {0x0c, 0x1c, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e},  // 1
    {0x7e, 0x7f, 0x03, 0x03, 0x3f, 0x7e, 0x60, 0x60, 0x7f, 0x7f},  // 2
    {0xfe, 0x7f, 0x03, 0x03, 0x7f, 0x7f, 0x03, 0x03, 0x7f, 0x7e},  // 3
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0x7f, 0x06, 0x06},  // 4
    {0x7f, 0x7f, 0x60, 0x60, 0x7e, 0x7f, 0x03, 0x03, 0x7f, 0x7e},  // 5
    {0x60, 0x60, 0x60, 0x60, 0x7e, 0x7f, 0x63, 0x63, 0x7f, 0x3e},  // 6
    {0x7f, 0x7f, 0x63, 0x63, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03},  // 7
    {0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x7f, 0x63, 0x63, 0x7f, 0x3e},  // 8
};

char *fpga_addr[FPGA_DEVICES_CNT];

void fpga_iomap_devices(void) {
    fpga dev;

    fpga_addr[DOT] = ioremap(DOT_ADDRESS, 0x10);
    fpga_addr[FND] = ioremap(FND_ADDRESS, 0x4);
    fpga_addr[LED] = ioremap(LED_ADDRESS, 0x1);
    fpga_addr[SWITCH] = ioremap(SWITCH_ADDRESS, 0x60);
    fpga_addr[TEXT_LCD] = ioremap(TEXT_LCD_ADDRESS, 0x32);

    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        if (fpga_addr[dev] == NULL) {
            printk("@@@ Failed to IO-map device @@@\n");
        }
        return;
    }
}

void fpga_iounmap_devices(void) {
    fpga dev;

    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        iounmap(fpga_addr[dev]);
    }
}

void fpga_dot_write(const int digit) {
    int i;

    for (i = 0; i < 10; i++) {
        outw(fpga_dot_digit[digit][i] & 0x7F, (unsigned int) fpga_addr[DOT] + i * 2);
    }
}

// fnd output
// parameter : 4 digits
void fpga_fnd_write(int num0, int num1, int num2, int num3) {
    unsigned short int value_short = 0;
    value_short = value_short + ((unsigned short int) num0 << 12);
    value_short = value_short + ((unsigned short int) num1 << 8);
    value_short = value_short + ((unsigned short int) num2 << 4);
    value_short = value_short + (unsigned short int) num3;
    outw(value_short, (unsigned int)fpga_addr[FND]);
}

// led output
// parameter : current runners on base, out count
void fpga_led_write(const int *base, const int out) {
    unsigned short value = 0;

    if (out >= 1) {
        value = 1;
    }
    if (out >= 2) {
        value += (1 << 4);
    }

    if (base != NULL) {
        if (base[0] == 1) {
            value += (1 << 1);
        }
        if (base[1] == 1) {
            value += (1 << 6);
        }
        if (base[2] == 1) {
            value += (1 << 3);
        }
    }
    else {
        value = 0;
    }

    outw(value, (unsigned int) fpga_addr[LED]);
}

// switch input
// Return : # of which switch is pressed
int fpga_switch_read(void) {
    int i;
    unsigned short int value;

    for (i = 0; i < SWITCH_CNT; i++) {
        value = inw((unsigned int) fpga_addr[SWITCH] + i * 2);
        if ((value & 0xFF) != 0) {
            return i + 1;
        }
    }

    return -1;
}

void fpga_text_lcd_write(int ball, int strike) {
    int i;
    unsigned short int value;
    unsigned char buffer[TEXT_LCD_BUFFER_SIZE + 1] = {0};

    memset(buffer, ' ', TEXT_LCD_BUFFER_SIZE);

    if (ball != -1) {
        buffer[0] = ball + '0';
        buffer[1] = 'B';
        buffer[3] = strike + '0';
        buffer[4] = 'S';
    }
    buffer[TEXT_LCD_BUFFER_SIZE] = '\0';

    for (i = 0; i < TEXT_LCD_BUFFER_SIZE; i += 2) {
        value = (buffer[i] & 0xFF) << 8 | (buffer[i + 1] & 0xFF);
        outw(value, (unsigned int) fpga_addr[TEXT_LCD] + i);
    }
}

void fpga_initialize(void) {
    fpga_dot_write(0);
    fpga_fnd_write(0, 0, 0, 0);
    fpga_led_write(0, 0);
    fpga_text_lcd_write(-1, -1);
}

void fpga_print(void) {
    fpga_dot_write(gameStatus->score);
    fpga_fnd_write(gameStatus->userNum[0], gameStatus->userNum[1], 
    gameStatus->userNum[2], gameStatus->userNum[3]);
    fpga_led_write(gameStatus->base, gameStatus->out);
    fpga_text_lcd_write(gameStatus->ball, gameStatus->strike);
}