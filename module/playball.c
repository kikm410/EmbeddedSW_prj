#include "numbaseball.h"

payload gameStatusInstance;
payload *gameStatus = &gameStatusInstance;

static irqreturn_t home_handler(int, void*);
static irqreturn_t back_handler(int, void*);

void inter_open(void) {
    int ret, irq;

    gpio_direction_input(HOME);
    irq = gpio_to_irq(HOME);
    ret = request_irq(irq, home_handler, IRQF_TRIGGER_RISING, "home", 0);

    gpio_direction_input(BACK);
    irq = gpio_to_irq(BACK);
    ret = request_irq(irq, back_handler, IRQF_TRIGGER_RISING, "back", 0);
}

void inter_release(void) {
    free_irq(gpio_to_irq(HOME), NULL);
    free_irq(gpio_to_irq(BACK), NULL);
}

static irqreturn_t home_handler(int irq, void* dev_id) {
    printk("HOME PRESSED\n");
    gameStatus->flag = 1;
    gameStatus->score = 10;    

    return IRQ_HANDLED;
}

static irqreturn_t back_handler(int irq, void* dev_id) {
    printk("current status\n");
    printk("base : %d%d%d\n", gameStatus->base[0], gameStatus->base[1], gameStatus->base[2]);
    printk("out : %d\n", gameStatus->out);        
    printk("score : %d\n", gameStatus->score);        
    printk("inning : %d\n", gameStatus->inning);       
    printk("flag : %d\n", gameStatus->flag);
    printk("hitternum : %d%d%d%d\n", gameStatus->hitterNum[0], gameStatus->hitterNum[1], gameStatus->hitterNum[2], gameStatus->hitterNum[3]);
    printk("usernum : %d%d%d%d\n", gameStatus->userNum[0], gameStatus->userNum[1], gameStatus->userNum[2], gameStatus->userNum[3]);
    printk("ball, strike : %dB%dS\n", gameStatus->ball, gameStatus->strike);

    return IRQ_HANDLED;
}

void initializePayload(void) {
    int i;

    for (i = 0; i < 3; i++) {
        gameStatus->base[i] = 0;
    }
    gameStatus->out = 0;
    gameStatus->score = 0;
    gameStatus->inning= 0;
    gameStatus->flag = 0;
    for (i = 0; i < 4; i++) {
        gameStatus->hitterNum[i] = 0;
        gameStatus->userNum[i] = 0;
    }
    gameStatus->ball = 0;
    gameStatus->strike = 0;
}

void startGame() {
    int i;

    initializePayload();
    fpga_initialize();

    while (gameStatus->score <= 4) {
        hitterEnter();

        while (gameStatus->flag == 0) {
            chooseBall();
            throwBall();
            for (i = 0; i < 4; i++) {
                gameStatus->userNum[i] = 0;
            }
        }

        hitterChange();
        inningChange();
        fpga_print();
    }

    printk("You survived %d innings!\n", gameStatus->inning);

    fpga_initialize();
}

void chooseBall() {
    int i, j;
    int temp;

    for (i = 0; i < 4; i++) {
        temp = fpga_switch_read();
        if (temp == -1) {
            i--;
        }
        else {
            for (j = 0; j < i; j++) {   // if duplicated num, try again
                if (temp == gameStatus->userNum[j]) {
                    temp = 0;
                    i--;
                }
            }
            if (temp != 0) {
                gameStatus->userNum[i] = temp;
            }
            msleep(500);
        }
        fpga_fnd_write(gameStatus->userNum[0], gameStatus->userNum[1], 
        gameStatus->userNum[2], gameStatus->userNum[3]);
    }
}

void throwBall() {
    int i, j;
    int check = 0;
    int count = 0;

    //check for direct match (out)
    for (i = 0; i < 4; i++) {
        if (gameStatus->userNum[i] == gameStatus->hitterNum[i]) {
            gameStatus->flag = 1;   // change hitter
            gameStatus->out += 1;
            return;
        }
    }

    // check for homerun
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (gameStatus->hitterNum[i] != gameStatus->userNum[j]) {
                check++;
            }
        }
    }
    if (check == 16) {
        gameStatus->flag = 1;
        for (i = 0; i < 3; i++) {
            if (gameStatus->base[i] == 1) {
                gameStatus->score++;
                gameStatus->base[i] = 0;
            }
        }
        gameStatus->score++;
        fpga_print();
        return;
    }

    // check for indirect match
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) {
                continue;
            }
            else {
                if (gameStatus->hitterNum[i] == gameStatus->userNum[j]) {
                    count++;
                }
            }
        }
    }

    switch (count) {
    case 1:
        gameStatus->flag = 1;
        if (gameStatus->base[2] == 1) {
            gameStatus->score++;
            gameStatus->base[2] = gameStatus->base[1];
            gameStatus->base[1] = gameStatus->base[0];
            gameStatus->base[0] = 1;
        }
        else {
            gameStatus->base[2] = gameStatus->base[1];
            gameStatus->base[1] = gameStatus->base[0];
            gameStatus->base[0] = 1;
        }
        break;
    case 2:
        gameStatus->ball++;
        if (gameStatus->ball == 4) {
            if (gameStatus->base[0] == 0) {
                gameStatus->base[0] = 1;
            }
            else {
                if (gameStatus->base[1] == 0) {
                    gameStatus->base[1] = 1;
                }
                else {
                    if (gameStatus->base[2] == 0) {
                        gameStatus->base[2] = 1;
                    }
                    else {
                        gameStatus->flag = 1;
                        gameStatus->score++;
                    }
                }
            }
        }
        break;
    case 3:
        gameStatus->strike++;
        if (gameStatus->strike == 3) {
            gameStatus->flag = 1;
            gameStatus->out++;
        }
        break;
    case 4:
        gameStatus->strike++;
        if (gameStatus->strike == 3) {
            gameStatus->flag = 1;
            gameStatus->out++;
        }
        break;
    default:
        break;
    }

    // fpga print
    fpga_print();
}

void hitterChange() {
    int i;
    for (i = 0; i < 4; i++) {
        gameStatus->userNum[i] = 0;
    }
    gameStatus->ball = 0;
    gameStatus->strike = 0;
    gameStatus->flag = 0;
}

void inningChange() {
    int i;

    if (gameStatus->out == 3) {
        gameStatus->out = 0;
        gameStatus->inning++;
        for (i = 0; i < 3; i++) {
            gameStatus->base[i] = 0;
        }
    }
}

void hitterEnter() {
    int i, j;
    int random;

    for (i = 0; i < 4; i++) {
        get_random_bytes(&random, sizeof(int));
        gameStatus->hitterNum[i] = (((unsigned int)random) % 9) + 1;
        for (j = 0; j < i; j++) {
            if (gameStatus->hitterNum[i] == gameStatus->hitterNum[j]) i--;
        }
    }

    printk("gerenated ran num : %d%d%d%d\n", gameStatus->hitterNum[0],
    gameStatus->hitterNum[1], gameStatus->hitterNum[2], gameStatus->hitterNum[3]);
}