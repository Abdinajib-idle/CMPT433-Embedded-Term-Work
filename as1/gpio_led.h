#ifndef GPIO_LED_H
#define GPIO_LED_H

// Define the Paths for the GPIO LEDs, had to manually config and set up the
#define LED1 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define LED2 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define LED3 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define LED4 "/sys/class/leds/beaglebone:green:usr3/brightness"

// Joystick GPIO value file paths (added these definitions)

#define GPIO_UP_VALUE "/sys/class/gpio/gpio26/value"
#define GPIO_DOWN_VALUE "/sys/class/gpio/gpio46/value"
#define GPIO_LEFT_VALUE "/sys/class/gpio/gpio65/value"
#define GPIO_RIGHT_VALUE "/sys/class/gpio/gpio47/value"
#define GPIO_CENTER_VALUE "/sys/class/gpio/gpio27/value"

// Function prototypes for GPIO and LED control
void initialize();
void turnOnLED(const char *led);
void turnOffLED(const char *led);
void controlLED(const char *led, const char *state);
void turnOffAllLEDS();
void flashLEDs(int hz, float durationMs);

// Function prototype for reading GPIO value (added this prototype)
int readGPIO(const char *path);

#endif