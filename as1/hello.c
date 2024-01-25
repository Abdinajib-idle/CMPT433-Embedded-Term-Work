#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// The Paths for the GPIO
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

// Function prototypes
void initialize();
void displayWelcomeMessage();
void playGame();
void turnOnLED(const char* led);
void turnOffLED(const char* led);
void waitForJoystickRelease();
int readJoystick();
void displayDirection(const char* direction);
void processResponse(int correctDirection);
void flashLEDs();
void flashLEDsRapidly();
void cleanup();

int main() {
    /*
    initialize();
    displayWelcomeMessage();
    playGame();
    cleanup();
    
   initialize();
   displayWelcomeMessage();
   turnOnLED(LED3);
   turnOnLED(LED4);
   sleep(5);
   displayWelcomeMessage();
   turnOffLED(LED3);
   turnOffLED(LED4);
   */
  initialize();
  printf("Testing JoyStick");
  // Initial setup, if any
    initialize();

    printf("Testing the joystick. Press CTRL+C to exit.\n");

    // Infinite loop to continuously read the joystick state
    while (1) {
        int direction = readJoystick();
        switch (direction) {
            case JOYSTICK_UP:
                printf("Up\n");
                break;
            case JOYSTICK_DOWN:
                printf("Down\n");
                break;
            case JOYSTICK_LEFT:
                printf("Left\n");
                break;
            case JOYSTICK_RIGHT:
                printf("Right\n");
                break;
            case JOYSTICK_CENTER:
                printf("Center\n");
                break;
            case JOYSTICK_NONE:
                // printf("No input\n"); // Uncomment if you want to see the "no input" state
                break;
            default:
                printf("Unknown\n");
        }
        sleep(1); // Delay a bit before reading again
    }
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
    // struct timespec ts = {0, 200000000L}; // 200ms delay

    // Disable the triggers for the LEDs
    for (int i = 0; i < 4; i++) {
        fd = open(ledTriggers[i], O_WRONLY);
        if (fd < 0) {
            perror("Error opening LED trigger file");
            exit(1);
        }
        write(fd, "none", 4);
        close(fd);
    }

    // Export and set direction for each joystick GPIO pin
    for (int i = 0; i < 5; i++) {
        // Construct the path to check if GPIO is already exported
        snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%s", joystickGPIOPins[i]);
        
        // Check if the GPIO is already exported
        if (access(directionPath, F_OK) == -1) {
            // Export the GPIO pin
            fd = open("/sys/class/gpio/export", O_WRONLY);
            if (fd < 0) {
                perror("Error exporting GPIO");
                exit(1);
            }
            if (write(fd, joystickGPIOPins[i], strlen(joystickGPIOPins[i])) != strlen(joystickGPIOPins[i])) {
                perror("Error writing to GPIO export");
                close(fd);
                exit(1);
            }
            close(fd);

            // Wait for the export to take effect
            sleep(200000);

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
            perror("Error opening GPIO direction file");
            exit(1);
        }
        if (write(fd, "in", 2) != 2) {
            perror("Error setting GPIO direction to input");
            close(fd);
            exit(1);
        }
        close(fd);
    }
}
void displayWelcomeMessage() {
    printf("Welcome to the LED and Joystick Game!\n\n");
    printf("Instructions:\n");
    printf("1. The game will indicate a direction (up or down).\n");
    printf("2. When you see the direction, move the joystick in that direction as quickly as possible.\n");
    printf("3. The LEDs will show the direction: top LED for up, bottom LED for down.\n");
    printf("4. If you move the joystick correctly, all LEDs will flash. If not, they will flash rapidly.\n");
    printf("5. Press the joystick to the left or right to exit the game at any time.\n");
    printf("6. The game will automatically end if no action is taken within 5 seconds.\n\n");
    printf("Get ready to play!\n");
}

void playGame() {
    // Main game loop
}
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
    // Wait until joystick is released
}



void displayDirection(const char* direction) {
    // Display chosen direction and update LEDs
}

void processResponse(int correctDirection) {
    // Process user's joystick press
}

void flashLEDs() {
    // Flash all four LEDs
}

void flashLEDsRapidly() {
    // Flash all four LEDs rapidly
}

void cleanup() {
    // Cleanup resources, turn off LEDs, etc.
}

//khsdfdsjhkl

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