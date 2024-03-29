#ifndef JOYSTICK_H
#define JOYSTICK_H
//joystick states
#define JOYSTICK_UP 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_RIGHT 4
#define JOYSTICK_CENTER 5
#define JOYSTICK_NONE 0
//funs
int readJoystick();
void waitForJoystickRelease();
int readGPIO(const char *path);

#endif