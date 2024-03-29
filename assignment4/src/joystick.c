#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "joystick.h" 

// down & right gpio values
/**
 * @jiu these are the values for my case please update it with yours if different
*/
#define GPIO_DOWN_VALUE "/sys/class/gpio/gpio46/value" 
#define GPIO_RIGHT_VALUE "/sys/class/gpio/gpio47/value" 

int readJoystick() {
    int down = readGPIO(GPIO_DOWN_VALUE);
    int right = readGPIO(GPIO_RIGHT_VALUE);

    if (down == 0) return JOYSTICK_DOWN;
    if (right == 0) return JOYSTICK_RIGHT;

    return JOYSTICK_NONE;
}

void waitForJoystickRelease() {
    int down, right;
    do {
        down = readGPIO(GPIO_DOWN_VALUE);
        right = readGPIO(GPIO_RIGHT_VALUE);

        if (down == 0 || right == 0) {
            printf("Please let go of the joystick.\n");
            sleepForMs(500);
        }
    } while (down == 0 || right == 0);
}

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
