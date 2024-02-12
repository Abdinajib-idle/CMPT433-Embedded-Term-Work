// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <adc.h>
#include <pwd.h>


int main()
{
    printf("Hello world!\n");

    // Initialize all modules; HAL modules first
    adc_init();
    pwm_init();

    while(1) {
        //main loop to control
    }
    return 0;
}