#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <game.h>
#include <gpio_led.h>
/*Main function seeds ran then initializes the game, displays welcome message then commences the game with the playGame look*/
int main() {
    srand(time(NULL)); //kept getting non-random when i used rand() found this on stackoverflow:https://stackoverflow.com/questions/52801380/srandtimenull-function 
    initialize();
    displayWelcomeMessage();
    playGame();
    return 0;
}