# wordtrain #
## A command-line game I wrote for kids in C++ (needs C++ 11 or higher) ##
### Computer and you take turns in playing a letter each. First player to complete a valid word (5 letters or longer) loses! ###

This README can guide you, step by step, if you want to:
* Download the code, compile and play the game
* Download and follow along a walk-through of the C++ code

#### Download the code, compile and play the game ####

1. My instructions are for Linux. On Windows, you can use an IDE like Visual Studio to compile and run the code; it might need some additional steps like setting of relative paths to find the word files for loading, using C++ 11 and STL libraries, etc.
2. On a Linux machine, clone this git repository
3. Make sure you can compile C++ with C++ 11 libraries installed
    1. On Debian derivatives like Ubuntu, this will help: `sudo apt-get install build-essential`
4. Navigate to the directory where you cloned this repository. if you list the files, you should see train.cpp and and subdirectory called wordsNOCR with some more files in it
5. Build the executable: `g++ -std=c++11 -o train train.cpp`
6. Run the executable: `./train`
7. Play, the instruction-set is self-explanatory, game play is easy

##### Notes on gameplay #####

1. The code focuses on core aspects like managing data structures to hold the words in memory and efficient searching. I have not spent a lot of time in making the game play experience super-smooth, user interaction can be improved vastly. My kids are happy when I take the effort to make something for them - and they would play for hours and try to learn and improve the code. Hence, I do not make them super-polished purposefully
2. The program only runs one game and then exits. You need to run it again to play again - which means it loads the dictionary files all over again when it re-runs. This behavior can be very easily changed to run in an infinite loop - see the notes in the code walk-through section
3. I have tried to dumb the program down - after all it is the computer, and it will (almost) always win if you make it consider every possibility. As a result, it sometimes acts super dumb
4. A popular version of this game on the IOS app store works for words 3 letters or longer. I find that too easy. To make it more challenging, I have changed the minimum length to 5


#### Download and follow along a walk-through of the C++ code ####

YYY
