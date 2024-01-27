#define _POSIX_C_SOURCE 200809L
#include <limits.h> //I added this to be able to use LONG_LONG_MAX source: https://learn.microsoft.com/en-us/cpp/c-language/cpp-integer-limits?view=msvc-170

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

// The Paths for t he GPIO, i went through the guides, tested all the leds i was interested
#define LED1 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define LED2 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define LED3 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define LED4 "/sys/class/leds/beaglebone:green:usr3/brightness"

// Joystick GPIO value file paths
#define GPIO_UP_VALUE "/sys/class/gpio/gpio26/value"
#define GPIO_RIGHT_VALUE "/sys/class/gpio/gpio47/value"
#define GPIO_DOWN_VALUE "/sys/class/gpio/gpio46/value"
#define GPIO_LEFT_VALUE "/sys/class/gpio/gpio65/value"
#define GPIO_CENTER_VALUE "/sys/class/gpio/gpio27/value"

#define JOYSTICK_UP 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_RIGHT 4
#define JOYSTICK_CENTER 5
#define JOYSTICK_NONE 0

//game states
#define GAME_CONTINUE 1
#define GAME_EXIT 0

// Function prototypes
void initialize();
void displayWelcomeMessage();
void playGame();
void turnOnLED(const char* led);
void turnOffLED(const char* led);
int readGPIO(const char* path);
void waitForJoystickRelease();
void controlLED(const char* led, const char* state);
int readJoystick();
void displayDirection(const char* direction);
int processResponse(int correctDirection, long long startTime, long long* bestTime);
void flashLEDs(int hz, float durationMs);
void flashLEDsRapidly();
void turnOffAllLEDS();
void handleCorrectResponse(long long* bestTime, long long responseTime);
void handleIncorrectResponse();
// time function prototypes to be used with the provided time functions in the assignment below
static void sleepForMs(long long delayInMs);
static long long getTimeInMs();

/*Code provided in the assigment instructions starts*/
static long long getTimeInMs(void){
 struct timespec spec;
 clock_gettime(CLOCK_REALTIME, &spec);
 long long seconds = spec.tv_sec;
 long long nanoSeconds = spec.tv_nsec;
 long long milliSeconds = seconds * 1000
 + nanoSeconds / 1000000;
 return milliSeconds;
}
static void sleepForMs(long long delayInMs)
{
 const long long NS_PER_MS = 1000 * 1000;
 const long long NS_PER_SECOND = 1000000000;
 long long delayNs = delayInMs * NS_PER_MS;
 int seconds = delayNs / NS_PER_SECOND;
 int nanoseconds = delayNs % NS_PER_SECOND;
 struct timespec reqDelay = {seconds, nanoseconds};
 nanosleep(&reqDelay, (struct timespec *) NULL);
}
/*Code provided in the assigment instructions ends here*/

int main() {
    srand(time(NULL)); //kept getting non-random when i used rand() found this on stackoverflow:https://stackoverflow.com/questions/52801380/srandtimenull-function 
    initialize();
    displayWelcomeMessage();
    playGame();
    return 0;
}
void initialize() {
    int fd;
    const char* ledTriggers[] = {
        "/sys/class/leds/beaglebone:green:usr0/trigger",
        "/sys/class/leds/beaglebone:green:usr1/trigger",
        "/sys/class/leds/beaglebone:green:usr2/trigger",
        "/sys/class/leds/beaglebone:green:usr3/trigger"
    };
    const char* joystickGPIOPins[] = {"26", "47", "46", "65", "27"};
    char directionPath[40];

    // step 1: Disable the triggers for the LEDs
    for (int i = 0; i < 4; i++) {
        fd = open(ledTriggers[i], O_WRONLY);
        if (fd < 0) {
            perror("Error opening LED trigger file");
            exit(1);
        }
        write(fd, "none", 4);
        close(fd);
    }

    // This exports and set direction for each joystick GPIO pin
    for (int i = 0; i < 5; i++) {
        // Construct the path to check if GPIO is already exported
        snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%s", joystickGPIOPins[i]);
        
        // Incase the GPIO is already exported
        if (access(directionPath, F_OK) == -1) {
            // Export the GPIO pin
            fd = open("/sys/class/gpio/export", O_WRONLY);
            if (fd < 0) {
                perror("Can't export GPIO");
                exit(1);
            }
            if (write(fd, joystickGPIOPins[i], strlen(joystickGPIOPins[i])) != strlen(joystickGPIOPins[i])) {
                perror("Failed writing to GPIO export");
                close(fd);
                exit(1);
            }
            close(fd);

            // Wait for a bit
            sleep(1000);

            // Check again if GPIO is exported
            if (access(directionPath, F_OK) == -1) {
                fprintf(stderr, "GPIO %s not exported after delay\n", joystickGPIOPins[i]);
                exit(1);
            }
        }

        // Set the GPIO pin direction to input
        snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%s/direction", joystickGPIOPins[i]);
        fd = open(directionPath, O_WRONLY);
        if (fd < 0) {
            perror("Can't open GPIO direction file");
            exit(1);
        }
        if (write(fd, "in", 2) != 2) {
            perror("Can't set GPIO direction to input");
            close(fd);
            exit(1);
        }
        close(fd);
    }
    srand(time(NULL));
}
void displayWelcomeMessage() {
    printf("Hello embedded world, from Abdinajib\n");
    printf("When the LEDs light up, press they joystick in that direction\n");
    printf(" (press left or right to exit)\n");
    printf("Get ready to play!\n");
        //turn on the middleLED's at the beginning
    turnOnLED(LED2);
    turnOnLED(LED2);
}
void playGame() {
    srand(time(NULL));
    /*Game variables--> for control, timing e.t.c.*/
    int running = 1;
    //directions
    int correctDirection, joystickDirection;
    //Time vars
    long long bestTime = LLONG_MAX; //see the citations at the top on source and use
    long long startTime, currentTime, responseTime; 

    while (running) {
        printf("Get ready!\n");
        //turn off all the LEDs first
        turnOffAllLEDS(); 
        //then turn on the middle 2 while the program will choose the direction
        controlLED(LED2, "1");
        controlLED(LED3, "1");
        //to ensure that game starts while it is in centre/neutral postion if not constantly ask to release it
        waitForJoystickRelease(); 

        long long waitTime = (rand() % 2500) + 500; // Random wait time
        sleepForMs(waitTime);

        correctDirection = (rand() % 2) + 1; // Random direction
        displayDirection(correctDirection == 1 ? "up" : "down");

        // Reset joystick direction before waiting for new input
        joystickDirection = JOYSTICK_NONE; 
        startTime = getTimeInMs();

        // Wait for joystick input or timeout
        do {
            joystickDirection = readJoystick();
            currentTime = getTimeInMs();
            if (joystickDirection != JOYSTICK_NONE || currentTime - startTime >= 5000) {
                break; // Break the loop if response or timeout
            }
        } while (true);

        // Handle the response
        if (joystickDirection != JOYSTICK_NONE) {
            responseTime = currentTime - startTime;
            if (joystickDirection == correctDirection) {
                // Correct response
                if (responseTime < bestTime) {
                    bestTime = responseTime;
                    printf("New best time: %lld ms\n", bestTime);
                }
                printf("Correct response!\nThis attempt's time: %lld ms\nBest time so far: %lld ms\n", responseTime, bestTime);
                printf("\n--------------->>>>\n");
                flashLEDs(4,500);
                // flashLEDs(); // Flash all LEDs for correct response
            } else if (joystickDirection == JOYSTICK_LEFT || joystickDirection == JOYSTICK_RIGHT) {
                // Exiting the game
                printf("Exiting the game.\n");
                running = 0;
            } else {
                // Incorrect response
                printf("Incorrect response!\n");
                flashLEDs(10,100);
                // flashLEDsRapidly(); // Flash all LEDs rapidly for incorrect response
            }
        } else if (currentTime - startTime >= 5000) {
            // Timeout without response
            printf("Time's up! No response received.\n");
            running = 0;
        }

        // Reset the LEDs and state as needed for the next round
        turnOffAllLEDS();
    }

    printf("Thank you for playing!\n");
}


// void handleCorrectResponse(long long* bestTime, long long responseTime) {
//     // If this is the best time so far, update the best time
//     if (responseTime < *bestTime) {
//         *bestTime = responseTime;
//         printf("\tNew best time: %lld ms\n", *bestTime);
//     }

//     // Display response times
//     printf("Correct response!\n");
//     printf("This attempt's time: %lld ms\nBest time so far: %lld ms\n", responseTime, *bestTime);
//     // Flash all LEDs to indicate success
//     flashLEDs();
// }


/*
*takes in path to @param led's brightness, opens it and write 1 to turn it on
*/
void turnOnLED(const char* led) {
    int fd = open(led, O_WRONLY);
    if(fd < 0)
    {
        perror("Wrong file to write to");
        return;
    }
    if(write(fd, "1", 1) !=1){
        perror("Failed to turn on the LED");
    }
    close(fd);
}
/*
*takes in path to @param led's brightness, opens it and write 1 to turn it on
*/
void turnOffLED(const char* led) {
    int fd = open(led, O_WRONLY);
    if(fd < 0)
    {
        perror("Wrong file to write to");
        return;
    }
    if(write(fd, "0", 1) !=1){
        perror("Failed to turn on the LED");
    }
    close(fd);
}

void waitForJoystickRelease() {
    int up, down, left, right, center;
    do {
        up = readGPIO(GPIO_UP_VALUE);
        down = readGPIO(GPIO_DOWN_VALUE);
        left = readGPIO(GPIO_LEFT_VALUE);
        right = readGPIO(GPIO_RIGHT_VALUE);
        center = readGPIO(GPIO_CENTER_VALUE);

        // If any direction is active, prompt user to release the joystick
        if (up == 0 || down == 0 || left == 0 || right == 0 || center == 0) {
            printf("Please let go of the joystick.\n");
            sleepForMs(500); // Provide some time for the user to release the joystick
        }
    } while (up == 0 || down == 0 || left == 0 || right == 0 || center == 0); // Loop until all directions are inactive
}


void displayDirection(const char* direction) {
    /*Diplay direction on the terminal*/
    printf("Move the joystick %s.\n", direction);
    /*Then update the led's to indicate direction*/

    if(strcmp(direction, "up") == 0)
    {
        controlLED(LED1, "1");
        controlLED(LED2, "0");
        controlLED(LED3, "0");
        controlLED(LED4, "0");
    }
    else if(strcmp(direction, "down") == 0)
    {
        controlLED(LED1, "0");
        controlLED(LED2, "0");
        controlLED(LED3, "0");
        controlLED(LED4, "1");
    
    }
}

// int processResponse(int correctDirection, long long startTime, long long* bestTime) {
//     long long currentTime = getTimeInMs();
//     long long responseTime = currentTime - startTime;
//     int userResponse = readJoystick();

//     if (userResponse == correctDirection) {
//         printf("Correct response!\n");
//         if (responseTime < *bestTime) {
//             *bestTime = responseTime;
//             printf("New best time: %lld ms\n", *bestTime);
//         }
//         printf("This attempt's time: %lld ms\nBest time so far: %lld ms\n", responseTime, *bestTime);
//         flashLEDs();  // Flash all LEDs as positive feedback
//     } else if (userResponse == JOYSTICK_LEFT || userResponse == JOYSTICK_RIGHT) {
//         printf("Exiting game.\n");
//         return GAME_EXIT;
//     } else {
//         printf("Incorrect response.\n");
//         flashLEDsRapidly();  // Flash all LEDs rapidly as negative feedback
//     }

//     return GAME_CONTINUE;
// }
/* Implementation of the controlLED function to turn an LED on or off*/
void controlLED(const char* led, const char* state) {
    int fd = open(led, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open the brightness file");
        return;
    }
    if (write(fd, state, strlen(state)) == -1) {
        perror("Failed to write to the brightness file");
    }
    close(fd);
}
// void flashLEDs() {
//     controlLED(LED1, "1");
//     controlLED(LED2, "1");
//     controlLED(LED3, "1");
//     controlLED(LED4, "1");
//     sleep(1000); // 1 second delay

//     controlLED(LED1, "0");
//     controlLED(LED2, "0");
//     controlLED(LED3, "0");
//     controlLED(LED4, "0");
//     sleep(1000); // 1 second delay
// }

// void flashLEDsRapidly() {
//         for (int i = 0; i < 5; i++) {
//         controlLED(LED1, "1");
//         controlLED(LED2, "1");
//         controlLED(LED3, "1");
//         controlLED(LED4, "1");
//         sleep(100); // 200 milliseconds delay

//         controlLED(LED1, "0");
//         controlLED(LED2, "0");
//         controlLED(LED3, "0");
//         controlLED(LED4, "0");
//         sleep(1000); // 200 milliseconds delay
//     }
// }




int readGPIO(const char* path) {
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
void turnOffAllLEDS() {
    controlLED(LED1, "0");
    controlLED(LED2, "0");
    controlLED(LED3, "0");
    controlLED(LED4, "0");
}

void flashLEDs(int hz, float durationMs) {
    int cycleMs = 1000/hz;
    int hafCycleMs = cycleMs/2;
    int numberCycles = durationMs/cycleMs;


for (int i = 0; i < numberCycles; i++) {
// Turn on all LEDs
controlLED(LED1, "1");
controlLED(LED2, "1");
controlLED(LED3, "1");
controlLED(LED4, "1");

// Sleep for half the period
sleepForMs(hafCycleMs);

// Turn off all LEDs
controlLED(LED1, "0");
controlLED(LED2, "0");
controlLED(LED3, "0");
controlLED(LED4, "0");

// Sleep for half the period
sleepForMs(hafCycleMs);
}
}