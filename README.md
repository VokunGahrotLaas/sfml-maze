# sfml-maze
C++ program which creates and solves mazes.\
This project has been greatly inspired by [angeluriot](https://github.com/angeluriot)'s project [Maze solver](https://github.com/angeluriot/Maze_solver).

## Usage
Usage: `sfml-maze [<maze y size>=144] [<update max fps>=960]`\
The y size of the maze will be raised to the nearest odd number (maze cannot have even number of tiles) (`<maze y size>`).\
The fps is vsynced but the max rate of calling of the update function can be controlled (`<update max fps>`).\
Advised sizes (for 16/9 screens): [144, 240, 360, 432, 450, 480, 504, 540, 576, 648, 720, 768, 900, 1080]\
Escape to quit.

## Compilation
 * Install the [SFML](https://www.sfml-dev.org/download/sfml/2.5.1/) library.
 * Install gcc ([MinGW-W64](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe) on windows).
 * Compile `main.cpp` with g++ (do not forget to link the SFML).

## Dependencies
 * [SFML](https://www.sfml-dev.org/index.php)

## Credits
 * Original project: [Maze solver](https://github.com/angeluriot/Maze_solver) by [angeluriot](https://github.com/angeluriot)
 * External library: [SFML](https://www.sfml-dev.org/index.php)