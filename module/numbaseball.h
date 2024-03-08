//stopwatch.h

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/wait.h>
#include <mach/gpio.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/random.h>
#include <linux/delay.h>

// for driver.c
#define DEVICE_MAJOR_NUMBER 242
#define DEVICE_NAME "numbaseball"
#define SUCCESS 0


// for fpga.c
#define FPGA_DEVICES_CNT 5
#define SWITCH_CNT 9
#define DOT_ADDRESS 0x08000210
#define FND_ADDRESS 0x08000004
#define LED_ADDRESS 0x08000016
#define SWITCH_ADDRESS 0x08000050
#define TEXT_LCD_ADDRESS 0x08000090

#define TEXT_LCD_BUFFER_SIZE 32

void fpga_iomap_devices(void);
void fpga_iounmap_devices(void);
void fpga_dot_write(const int);
void fpga_fnd_write(int, int, int, int);
void fpga_led_write(const int*, const int);
int fpga_switch_read(void);
void fpga_text_lcd_write(int, int);
void fpga_initialize(void);
void fpga_print(void);

typedef enum _FPGA_DEVICES { DOT, FND, LED, SWITCH, TEXT_LCD } fpga;

//for playball.c
#define HOME IMX_GPIO_NR(1, 11)
#define BACK IMX_GPIO_NR(1, 12)

typedef struct _payload {
    int base[3];   // base with runner
    int out;    // out count
    int score;  // baseball score (shown in fpga_dot)
    int inning;  // # of rounds player survived
    int flag;   // flag to indicate hitter change
    int hitterNum[4];   // random 4 digit
    int userNum[4]; // user 4 digit
    int ball;   //ball count
    int strike; // strike count
} payload;

void initializePayload(void);
void startGame(void);
void chooseBall(void);
void throwBall(void);
void hitterChange(void);
void inningChange(void);
void hitterEnter(void);
void inter_open(void);
void inter_release(void);