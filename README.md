# Tiny Dog Movement Controller

## Use these resources to set up for using this project

follow guides at:

https://blog.smittytone.net/2021/02/02/program-raspberry-pi-pico-c-mac/

then recommended C++ debugging setup

https://blog.smittytone.net/2021/02/05/how-to-debug-a-raspberry-pi-pico-with-a-mac-swd/

## Theory of Operation

Over SPI connection, commands are accepted from main controller detailing positions for points without regard for the actual servo angles or triginomotry required to get there. This allows the controller to take a large amount of the computational load, freeing up the main baord to run it's other supporting programs more effeciently. Simple commands will also be accepted that will allow for even simpler high level control. 
