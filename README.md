# AimBooster in C++
inspired by: [AimBooster](http://www.aimbooster.com/)

AimBooster is a fun and challenging game written in C++ using the SFML library. The game is designed to help players improve their aim and reaction times, offline.

## Gameplay

In AimBooster, targets pop up on the screen at random locations. The player's task is to hit these targets as quickly as possible. If you miss 3 times, it's game over.

## How to Play

1. Clone the repository.
### Or
Compile the game and link the libraries using your preferred C++ compiler(shown here using g++):
+ Run `g++ -c game9.cpp -I"path\\to\\repo\\SFML-2.5.1\\include" -DSFML-STATIC`
+ Run `g++ game9.o -o game9 -L"path\\to\\repo\\SFML-2.5.1\\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main`
Read more about this [here](https://www.sfml-dev.org/tutorials/2.5/)
2. Run the `game9.exe` file.
   
## Built With

- C++
- [SFML](https://www.sfml-dev.org/) - Simple and Fast Multimedia Library

## Note
This game was created as a personal project to allow me to play the original aimbooster.com game even when I did not have internet access. 
It turns out you can play aimbooster.com offline just by downloading [this file](http://www.aimbooster.com/faq#offline) and dragging it onto your browser. 
Nonetheless, it was a great exercise in learning basic graphics techniques in C++. I plan to work on this game more if I become interested in C++ graphics again.

## License

This project is licensed under the Apache 2.0 License - see the [LICENSE.md](LICENSE.md) file for details.
