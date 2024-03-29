cmake_minimum_required(VERSION 3.18.4)


project(HelloEmbedded VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(-Wall -Werror -Wextra)


include_directories(.)

add_executable(HelloEmbedded
    main.c
    game.c
    gpio_led.c
    joystick.c
    utils.c
)

