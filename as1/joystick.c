#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <fcntl.h>
#include <unistd.h>
#include "joystick.h"
#include "gpio_led.h"

int readJoystick() {
    int up = readGPIO(GPIO_UP_VALUE);
    int down = readGPIO(GPIO_DOWN_VALUE);
    int left = readGPIO(GPIO_LEFT_VALUE);
    int right = readGPIO(GPIO_RIGHT_VALUE);
    int center = readGPIO(GPIO_CENTER_VALUE);

    if (up == 0) return JOYSTICK_UP;
    if (down == 0) return JOYSTICK_DOWN;
    if (left == 0) return JOYSTICK_LEFT;
    if (right == 0) return JOYSTICK_RIGHT;
    if (center == 0) return JOYSTICK_CENTER;

    return JOYSTICK_NONE;
}
/*Func to check if gpio isn't in neutral pos if not ask player to let it go*/
void waitForJoystickRelease() {
    int up, down, left, right, center;
    do {
        up = readGPIO(GPIO_UP_VALUE);
        down = readGPIO(GPIO_DOWN_VALUE);
        left = readGPIO(GPIO_LEFT_VALUE);
        right = readGPIO(GPIO_RIGHT_VALUE);
        center = readGPIO(GPIO_CENTER_VALUE);

        if (up == 0 || down == 0 || left == 0 || right == 0 || center == 0) {
            printf("Please let go of the joystick.\n");
            sleepForMs(500);
        }
    } while (up == 0 || down == 0 || left == 0 || right == 0 || center == 0);
}
/*Func to read gpio*/
int readGPIO(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open GPIO value file");
        return -1;
    }
    char value_str[3];
    if (read(fd, value_str, 3) < 0) {
        perror("Failed to read GPIO value");
        close(fd);
        return -1;
    }
    close(fd);
    return atoi(value_str);
}