#include <stdio.h>
#include <limits.h> //I added this to be able to use LONG_LONG_MAX source: https://learn.microsoft.com/en-us/cpp/c-language/cpp-integer-limits?view=msvc-170
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "gpio_led.h"
#include "joystick.h"
#include "utils.h"

//function to display the welcome message upon starting the game
void displayWelcomeMessage() {
    printf("Hello embedded world, from Abdinajib\n");
    printf("When the LEDs light up, press the joystick in that direction\n");
    printf(" (press left or right to exit)\n");
    printf("Get ready to play!\n");
    turnOnLED(LED2);
    turnOnLED(LED3);
}

//this will contain the game logic, has the while loop to progress the game
void playGame() {
    int running = 1;
    int correctDirection, joystickDirection;
    long long bestTime = LLONG_MAX;
    long long startTime, currentTime, responseTime;

    while (running) {
        printf("Get ready!\n");
        turnOffAllLEDS();
        controlLED(LED2, "1");
        controlLED(LED3, "1");
        waitForJoystickRelease();

        long long waitTime = (rand() % 2500) + 500;
        sleepForMs(waitTime);
        /*
        randomly select a direct options: [up, dow]*/
        correctDirection = (rand() % 2) + 1;
        displayDirection(correctDirection == 1 ? "up" : "down");
        /* Resetting the joystick dir */
        joystickDirection = JOYSTICK_NONE;
        startTime = getTimeInMs();
        /*Loop to check curr pos of joystick using readJoystick fun
        *if not in neutral pos ask please let go message
        */
        do {
            joystickDirection = readJoystick();
            currentTime = getTimeInMs();
            if (joystickDirection != JOYSTICK_NONE || currentTime - startTime >= 5000) {
                break;
            }
        } while (true);
        /*
        *Processing input
        */
        if (joystickDirection != JOYSTICK_NONE) {
            responseTime = currentTime - startTime;
            if (joystickDirection == correctDirection) {
                if (responseTime < bestTime) {
                    bestTime = responseTime;
                    printf("New best time: %lld ms\n", bestTime);
                }
                printf("Correct response!\nThis attempt's time: %lld ms\nBest time so far: %lld ms\n", responseTime, bestTime);
                printf("\n--------------->>>>\n");
                flashLEDs(4, 500);
            } else if (joystickDirection == JOYSTICK_LEFT || joystickDirection == JOYSTICK_RIGHT) {
                printf("Exiting the game.\n");
                running = 0;
            } else {
                printf("Incorrect response!\n");
                flashLEDs(10, 100); //flashing leds using 10hz as per the instructions
            }
        } else if (currentTime - startTime >= 5000) {
            printf("Time's up! No response received.\n"); //timeout
            running = 0; //stopping the game
        }

        turnOffAllLEDS();//reset the leds so we restart we can just turn on the middle leds
    }

    printf("Thank you for playing!\n");
}
//diplaying dir using the leds
void displayDirection(const char *direction) {
    printf("Move the joystick %s.\n", direction);
    if (strcmp(direction, "up") == 0) {
        controlLED(LED1, "1");
        controlLED(LED2, "0");
        controlLED(LED3, "0");
        controlLED(LED4, "0");
    } else if (strcmp(direction, "down") == 0) {
        controlLED(LED1, "0");
        controlLED(LED2, "0");
        controlLED(LED3, "0");
        controlLED(LED4, "1");
    }
}

