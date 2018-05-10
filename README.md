# OpenGL School Project
A very simple FPS game.

## Compiling
### GNU/Linux
You first need to install OpenGL, GLEW, and SFML. Following commands have been used on Ubuntu 17.10.
```
$ sudo apt update
$ sudo apt install freeglut3-dev libglew-dev libsfml-dev
```
Download SOIL from the [official website](http://www.lonesock.net/soil.html).
Extract the archive, navigate to `projects/makefile` and create a folder named `obj`.
To compile and install SOIL:
```
$ make
$ sudo make install
```
Compile the main program
```
$ g++ -std=c++11 main.cpp -o main.out -lSOIL -lGL -lGLU -lGLEW -lsfml-graphics -lsfml-window -lsfml-system
```
