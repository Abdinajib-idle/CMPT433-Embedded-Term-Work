#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

// For atomic_bool
#include <stdatomic.h> 

// Function to initialize the 14-segment display and related I2C communication
void displayInit(void);

// Function to display the player's score on the 14-segment display
// @param score The player's score to be displayed, expected to be in the range of 0-99
void displayScore(int score);

// Function to clean up and properly close the display and I2C communication on program exit
void displayCleanup(void);

#endif // SEGMENT_DISPLAY_H
