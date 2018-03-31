# openGL_schoolproject

## Compiling
### Linux
You first need to install all the basic dependencies. Following commands have been used on Ubuntu 17.10.
```
sudo apt-get update
sudo apt-get install freeglut3
sudo apt-get install freeglut3-dev
sudo apt-get install binutils-gold
sudo apt-get install g++ cmake
sudo apt-get install libglew-dev
sudo apt-get install g++
sudo apt-get install mesa-common-dev
sudo apt-get install build-essential
sudo apt-get install libglew1.5-dev libglm-dev
```
Download SOIL from their [website](http://www.lonesock.net/soil.html).
Extract the archive, navigate to `projects/makefile' and create a folder named 'obj'.
To compile and install SOIL:
```
make
sudo make install
```
Compile the main program
```
g++ -std=c++11 main.cpp -o main -lGLEW -lSOIL -lGLU -lGL -lglut -lsfml-graphics -lsfml-window -lsfml-system
```
