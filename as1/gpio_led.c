#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "gpio_led.h"
#include "utils.h"

/*Fun to initi leds, gpios etc
*/
void initialize() {
    int fd;
    const char *ledTriggers[] = {
        "/sys/class/leds/beaglebone:green:usr0/trigger",
        "/sys/class/leds/beaglebone:green:usr1/trigger",
        "/sys/class/leds/beaglebone:green:usr2/trigger",
        "/sys/class/leds/beaglebone:green:usr3/trigger"
    };
    const char *joystickGPIOPins[] = {"26", "47", "46", "65", "27"};
    char directionPath[40];

    // Disable the triggers for the LEDs
    for (int i = 0; i < 4; i++) {
        fd = open(ledTriggers[i], O_WRONLY);
        if (fd < 0) {
            perror("Error opening LED trigger file");
            exit(EXIT_FAILURE);
        }
        if (write(fd, "none", 4) != 4) {
            perror("Failed to write 'none' to trigger");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
    }

    // Export and set direction for each joystick GPIO pin
    for (int i = 0; i < 5; i++) {
        snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%s", joystickGPIOPins[i]);

        if (access(directionPath, F_OK) == -1) {
            fd = open("/sys/class/gpio/export", O_WRONLY);
            if (fd < 0) {
                perror("Can't export GPIO");
                exit(EXIT_FAILURE);
            }
            if (write(fd, joystickGPIOPins[i], strlen(joystickGPIOPins[i])) != (ssize_t)strlen(joystickGPIOPins[i])) {
                perror("Failed writing to GPIO export");
                close(fd);
                exit(EXIT_FAILURE);
            }
            close(fd);
            sleepForMs(1000);

            if (access(directionPath, F_OK) == -1) {
                fprintf(stderr, "GPIO %s not exported after delay\n", joystickGPIOPins[i]);
                exit(EXIT_FAILURE);
            }
        }

        snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%s/direction", joystickGPIOPins[i]);
        fd = open(directionPath, O_WRONLY);
        if (fd < 0) {
            perror("Can't open GPIO direction file");
            exit(EXIT_FAILURE);
        }
        if (write(fd, "in", 2) != 2) {
            perror("Can't set GPIO direction to input");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
}
/*Func to turn on specific leds using controlLED function*/
void turnOnLED(const char *led) {
    controlLED(led, "1");
}
/*Func to turn on specific leds using controlLED function*/
void turnOffLED(const char *led) {
    controlLED(led, "0");
}
/*Func to turn on or off a target led*/
void controlLED(const char *led, const char *state) {
    int fd = open(led, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open the brightness file");
        exit(EXIT_FAILURE);
    }
    if (write(fd, state, strlen(state)) != (ssize_t)strlen(state)) {
        perror("Failed to write to the brightness file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}
/*Func to turn off all leds*/
void turnOffAllLEDS() {
    turnOffLED(LED1);
    turnOffLED(LED2);
    turnOffLED(LED3);
    turnOffLED(LED4);
}
/*Func to flash leds has @param hz and @durationMS which is durantion in ms*/
void flashLEDs(int hz, float durationMs) {
    int cycleMs = 1000 / hz;
    int halfCycleMs = cycleMs / 2;
    int numberCycles = (int)(durationMs / cycleMs);

    for (int i = 0; i < numberCycles; i++) {
        turnOnLED(LED1);
        turnOnLED(LED2);
        turnOnLED(LED3);
        turnOnLED(LED4);
        sleepForMs(halfCycleMs);
        turnOffAllLEDS();
        sleepForMs(halfCycleMs);
    }
}