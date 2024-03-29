#ifndef JOYSTICK_H
#define JOYSTICK_H

// Joystick movement definitions
#define JOYSTICK_NONE 0
#define JOYSTICK_DOWN 1
#define JOYSTICK_RIGHT 2

// Function prototypes
int readJoystick(void);
void waitForJoystickRelease(void);
int readGPIO(const char *path);

#endif
