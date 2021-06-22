# simul_furuta
A fun simulator of a Furuta Pendulum using pthread and allegro libraries.
The physics of the pendulum are based on the [Quanser Rotary Inverted Pendulum](https://www.quanser.com/products/rotary-inverted-pendulum/). The controllers, swing up and reference angle follower, are built in Matlab and exported through the Simulink Coder. All keyboard inputs plus some realtime statistics are displayed on the left of the application. 

## Install dependecies
It needs allegro 4.4, to install it:

`sudo apt install liballegro4.4 liballegro4-dev allegro4-doc`

## Compile
To compile just run in the main folder:

`cmake CMakeLists.txt`

`make`

## Launch
To run the simulator you'll need su privileges are needed to create real-time threads:
`sudo ./simul_furuta`

## TO DO
It remains to translate all the code and the final application in English!
