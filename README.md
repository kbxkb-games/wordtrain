# wordtrain #
## A command-line game I wrote for kids in C++ (needs C++ 11 or higher) ##
### Computer and you take turns in playing a letter each. First player to complete a valid word (5 letters or longer) loses! ###

This README can guide you, step by step, if you want to:
* Download the code, compile and play the game
* Download and follow along a walk-through of the C++ code

#### License - MIT (https://opensource.org/licenses/MIT) ####

#### Download the code, compile and play the game ####

1. My instructions are for Linux. On Windows, you can use an IDE like Visual Studio to compile and run the code; it might need some additional steps like setting of relative paths to find the word files for loading, using C++ 11 and STL libraries, etc.
2. On a Linux machine, clone this git repository
3. Make sure you can compile C++ with C++ 11 libraries installed
    1. On Debian derivatives like Ubuntu, this will help: `sudo apt-get install build-essential`
4. Navigate to the directory where you cloned this repository. if you list the files, you should see train.cpp and a subdirectory called wordsNOCR with some more files in it
5. Build the executable: `g++ -std=c++11 -o train train.cpp`
6. Run the executable: `./train`
7. Play, the instruction-set is self-explanatory, game play is easy

##### Notes on gameplay #####

1. The code focuses on core aspects like managing data structures to hold the words in memory and efficient searching. I have not spent a lot of time in making the game play experience super-smooth, user interaction can be improved vastly. My kids are happy when I take the effort to make something for them - and they would play for hours and try to learn and improve the code. Hence, I do not make them super-polished purposefully
2. The program only runs one game and then exits. You need to run it again to play again - which means it loads the dictionary files all over again when it re-runs. This behavior can be very easily changed to run in an infinite loop
3. I have tried to dumb the program down - after all it is the computer, and it will (almost) always win if you make it consider every possibility. As a result, it sometimes acts super dumb
4. A popular version of this game on the IOS app store works for words 3 letters or longer. I find that too easy. To make it more challenging, I have changed the minimum length to 5
5. Maximum word length is set to 20 - plenty actually


#### Follow along a walk-through of the C++ code ####

1. Before walking through the C++ code, let me quickly walk you through the dictionary files. This is my source of dictionary files: https://github.com/dwyl/english-words
    1. The only file you will need from the repo linked above is words_alpha.txt
    2. I used `sed` to remove all the carriage returns ('\r' characters): ` sed -i 's/\r//g' words_alpha.txt`
    3. I copied the file to a subdirectory called "WordsNOCR", the CPP code uses this relative path to load the words into memory at startup
    4. Inside WordsNOCR, I have a "scripts" subdirectory. It has 2 shell scripts. All we will need is the one that says, "split_words_by_length.sh"
    5. I ran this script - it expects a parameter - the words_alpha.txt file. It uses sed to break the file into smaller sub-files, each with words of a specific length. It starts from 5, and goes all the way upto 31, which is the length of the longest word in the big file: `./scripts/split_words_by_length.sh words_alpha.txt`
    6. The C++ code uses these smaller files to load the words into memory - organizing them into 16 std::set objects, where each set has all the words of a particular length, from length 5 through length 20
2. The C++ code uses a custom class `allWords` to wrap these 16 std::set-s, and a global object (`aw`) to store these words in heap. The constructor of this class uses ifstream to read each smaller file and populate the individual sets. The addresses of these 16 sets are stored in another set, sorted by the length of the words (from 5 to 20). The destructor of this class releases the memory used by these set objects
3. The code is easy to read through - start with `main()`, which provides user interface control flow in terms of user choosing whether they want to start or they want the computer to start, prints instruction/ usage rules, and eventually calls out to one of these two routines: `GamePlayUserStarts` and `GamePlayComputerStarts` to delegate finer control over game play
4. The two routines `GamePlayUserStarts` and `GamePlayComputerStarts` are slightly repetetive, and I believe that they can be combined for optimizing code length. However, they both sport while loops and either waits for the user to play a letter or asks the computer to play a letter if it is the computer's turn. The function where the computer serves the next letter is `GetNextChar(...)`
5. `GetNextChar` creates a `std::vector` of possible potential lengths it must target. If the word played so far is "star", it must target these lengths: 6, 8, 10, 12, 14, 16, 18 and 20 to win. I it targets a word that is say 5 or 7 characters long, it may be forced to complete the word, and thereby lose
6. Within this vector, it randomly picks a length, and calls out to `SearchTargetLength()` for that particular length. This it repeats as many times as necessary to come up with a good guess (but I have limited this number of efforts to 10, so that it also knows when to give up and concede defeat)
7. `SearchTargetLength()` only works on a given length of words. It first creates a subset of words that begins with the current word (**Example** - if the current word being played is "pre", and the length of words it is looking through is 9, it will create a subset of 9-long words that begin with "pre". However, it will exclude those words which has a substring of the *wrong length* that might result in the word being terminated prematurely (e.g., if it were to pick "startle", it would never reach the end, as "start" will finish the game). Once this subset is created (implemented as std::vector), it randomly picks a word from this vector
8.  The computer does not remember what it picked the last time. There is an exception to this - the *only* time computer remembers what it picked last time is if the user plays the exact character it wanted her/him to play when it is their turn.
    1. **Example** - current string is "dep". Computer picks "deprecate", hence plays 'r', making the current string "depr". Then, user plays 'e', making the current string "depre" - this is exactly what the computer had wanted, hence it will continue to play along that channel, without trying to come up with a different word, as it knows it will win if this word is followed till the last. However, if user changes course and plays "i" after "depr", the program is forced to forget its past choice and starts all over again to find a new target
9. Finally, where does the "dumbing down" happen? The randomness described in step 6 above contributes heavily to this dumbing down. This is because of two reasons:
    1. The randomeness ensures that the computer does not have full visibility into all possibilities. Say, it picks the length 10, and hence proceeds to target a word of length 10. There may (and most likley, will) exist a lot of possibilities with lengths less than 10, which the human user can then exploit. Similarly, even within length 10, it picks one word randomly, not making any effort to pick the *best* word that will increase the chances of winning
8. Feel free to download & improve!

### Future Improvements to code and functionaity ###

1. The code should be organized to be production-quality
    1. The class `allwords` should have its own header and implementation files, along with makefiles to tie all object files together while linking to create the executable. This breaks down monolithic long CPP files (mine is almost 500 lines long) - and increases both readability and maintainability
    2. Arbitrary constants (like the number of times the program tries to come up with the right target length, currently set to 10) should be removed from the code and pre-processing directives added to a global header file - again, increases readability and maintainability
    3. I am a fan of having a separate C/CPP file to initialize global and static variables (I have two global variables and a whole lot of static strings) - but I have not taken the time to do such changes here
2. Exception and error handling should be improved (if the dictionary files are not present, it does not throw any exception now, as ifstream object exception masks are not set properly)
3. Logging should be improved
4. Running comments should be improved
5. Functionality-wise, computer's "dumbness" can be made configurable (easy/medium/hard levels can be introduced) - and for the most difficult levels, the program can look through all different possibilities before playing
6. The program can be changed to keep on playing (and not quit after one round)
7. My favorite - we can give the game a proper GUI to make it more appealing to all kids
8. The words in the dictionary can be better curated. The current one has so many super-difficult words that gameplay sometimes gets weird
9. Please suggest more improvements!
