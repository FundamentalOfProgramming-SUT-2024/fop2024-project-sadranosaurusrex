#!/bin/bash

# Compile the C files with required libraries
gcc main.c menu.c UserFileCreator.c enemy.c map_plot.c -lm -lncursesw -lSDL2 -lSDL2_mixer -o my_game

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Run ./my_game to start the game."
else
    echo "Compilation failed. Check for errors."
fi
