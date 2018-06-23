#include <iostream>
#include <set>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <exception>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

class Formatter
{
public:
	Formatter() {}
	~Formatter() {}

	template <typename Type>
	Formatter & operator << (const Type & value)
	{
		stream_ << value;
		return *this;
	}

	std::string str() const		{return stream_.str();}
	operator std::string () const	{return stream_.str();}

	enum ConvertToString
	{
		to_str
	};
	std::string operator >> (ConvertToString) {return stream_.str();}
private:
	std::stringstream stream_;
	Formatter(const Formatter &);
	Formatter & operator = (Formatter &);
};

vector<string> KBPlays;

class allWords
{
public:
	allWords();
	~allWords();
	void debug_printCount();
	bool IsValidWord(string strWord);

	set<string>* operator [](int i) const	{return arrWordSets[i];}
	set<string>* & operator [](int i)	{return arrWordSets[i];}
private:
	set<string>* arrWordSets [16];
} aw;

allWords::allWords(void)
{
	try
	{
		string filename, word;
		for( int i = 0; i <= 15; i++ )
		{
			filename = "wordsNOCR/" + std::to_string(i+5) + "_length_words";
			ifstream filestream(filename.c_str());
			arrWordSets[i] = new set<string>();
			while(getline(filestream, word))
			{
				arrWordSets[i]->insert(word);
			}
		}
	}
	catch(exception& e)
	{
		throw runtime_error(Formatter() << "Standard exception in allWords constructor: " << e.what());
	}
};

allWords::~allWords(void)
{
	for( int i = 0; i <= 15; i++ )
	{
		delete arrWordSets[i];
	}
};

bool allWords::IsValidWord(string strWord)
{
	int len = strWord.size();
	return (len >= 5) && (len <= 20) && (aw[len - 5]->find(strWord) != aw[len - 5]->end());
}

void allWords::debug_printCount()
{
	for( int i = 0; i <= 15; i++ )
	{
		cout << "Number of words of length " << i + 5 << " - " << arrWordSets[i]->size() << endl;
	}
};

class findbeginswith
{
public:
	findbeginswith(string& beginswith) {_beginswith = beginswith;}
	bool operator()(const string& s) {return (s.compare(0, _beginswith.length(), _beginswith) == 0);}
private:
	string _beginswith;
};

char SearchTargetLength(string strWord, int targetlen, string& targetword)
{
	set<string>::iterator it = std::find_if(aw[targetlen - 5]->begin(), aw[targetlen - 5]->end(), findbeginswith(strWord));
	if(it == aw[targetlen - 5]->end())
		return '!';
	else
	{
		vector<string> matches;
		for(; it != aw[targetlen - 5]->end(); ++it)
		{
			if((*it).compare(0, strWord.length(), strWord) == 0)
			{
				bool bSubstringDanger = false;
				int checksubstr = strWord.length() + 1;
				while(checksubstr < 5) checksubstr += 2;
				while(checksubstr < targetlen)
				{
					if(aw.IsValidWord((*it).substr(0, checksubstr)))
					{
						bSubstringDanger = true;
						break;
					}
					checksubstr += 2;
				}
				if(!bSubstringDanger)
					matches.push_back(*it);
			}
			else
			{
				break;
			}
		}
		if(matches.size() <= 0)
			return '!';
		else
		{
			targetword = matches[(static_cast<long int>(time(NULL))) % matches.size()];
			KBPlays.push_back(targetword.substr(0, strWord.length() + 1) + " - " + targetword);
			return targetword[strWord.length()];
		}
	}
};

//the out parameter targetword will be populated with the word computer has in mind when it plays the next character
//however, this also means that if it populated while coming in, it is what the computer HAD IN MIND THE LAST TIME
//Therefore, we will use this knowledge to inject "state" - i.e., we will compare if the human user has played something
//that is along the same word that the computer had in mind last time. If yes, we will not change it - EVIL LAUGHTER
char GetNextChar(string strWord, string& targetword)
{
	int len = strWord.size();
	if(len <= 0)
		return ('a' + static_cast<long int>(time(NULL)) % 26);

	if(len >= 19) return '?';

	if(targetword.length() > 0 && targetword.compare(0, strWord.length(), strWord) == 0)
	{
		KBPlays.push_back(targetword.substr(0, strWord.length() + 1) + " - " + targetword);
		return targetword[strWord.length()];
	}

	vector<int> targetlens;
	int targetlen = len;
	do{targetlen += 2;} while (targetlen < 5);
	while(targetlen <= 20)
	{
		targetlens.push_back(targetlen);
		targetlen += 2;
	}
	//I have potential target lengths to search in the vector now
	//I will try ten times
	for(int i = 1; i <=10; i++)
	{
		int targetlenindex = (static_cast<long int>(time(NULL))) % targetlens.size();
		targetlen = targetlens[targetlenindex];
		char ret = SearchTargetLength(strWord, targetlen, targetword);
		if (ret != '!')
		{
			return ret;
		}
		else
		{
			targetlens.erase(targetlens.begin() + targetlenindex);
			if(targetlens.size() <= 0) break;
		}
	}
	//I am here means computer is ready to give up
	return '?';
};

void GamePlayUserStarts(void)
{
	char userInput = '~';
	string curWord = "";
	bool bGameEnded = false;
	string computerTargetWord;

	while(!bGameEnded)
	{
		userInput = getchar();
		putchar(' ');
		if(userInput < 97 || userInput > 122)
		{
			cout << endl << endl << "Only lowercase letters are allowed. Check your CAPS lock. YOU LOSE!!!!" << endl;
			bGameEnded = true;
			break;
		}

		curWord += userInput;
		if(aw.IsValidWord(curWord))
		{
			cout << endl << endl << curWord << " is a valid word, and guess who completed it? I win - YOU LOSE!" << endl << endl;
			bGameEnded = true;
			break;
		}
		else if(curWord.size() >= 20)
		{
			cout << endl << endl << "Too long. We will limit the number of letters to 20. GAME TIED" << endl << endl;
			bGameEnded = true;
			break;
		}

		char computerPlay = GetNextChar(curWord, computerTargetWord);
		putchar(computerPlay);
		putchar(' ');
		if(computerPlay == '?')
		{
			cout << endl << endl << "I am ready to give up. if you have a valid word in mind, you will win. But if you are bluffing, I will win." << endl;
			cout << "What word do you have in mind? (Use lower case alphabets only)" << endl;
			cout << "CAUTION - Due to the fact that I have done some terminal magic, BACKSPACE WON'T WORK. Try to type carefully!" << endl;
			string usertargetword;
			getline( cin, usertargetword);
			if(usertargetword.length() > 20)
			{
				cout << endl << endl << "Sorry, that word you had in mind is too long! This game allows up to 20 characters per word. YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
			else if(usertargetword.compare(0, curWord.length(), curWord) != 0)
			{
				cout << endl << endl << "Sorry, \"" << usertargetword << "\" does not start with \"" << curWord << "\". Where's your mind? YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
			else if(aw.IsValidWord(usertargetword))
			{
				cout << endl << endl << "Aha! Clever! \"" << usertargetword << "\" is a valid word, did not see that coming! YOU WIN!!!" << endl;
				bGameEnded = true;
				break;
			}
			else
			{
				cout << endl << endl << "Sorry, \"" << usertargetword << "\" is not a valid word. YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
		}
		curWord += computerPlay;
		if(aw.IsValidWord(curWord))
		{
			cout << endl << endl << "Oops. I completed \"" << curWord << "\", which is a valid word. YOU WIN! I lose" << endl << endl;
			bGameEnded = true;
			break;
		}
		else if(curWord.size() >= 20)
		{
			cout << endl << endl << "Too long. We will limit the number of letters to 20. GAME TIED" << endl << endl;
			bGameEnded = true;
			break;
		}
	}
	if(KBPlays.size() > 0)
	{
		cout << endl << "The words I had in mind when I played are: " << endl;
		for(int k = 0; k < KBPlays.size(); k++)
		{
			cout << KBPlays[k] << endl;
		}
		cout << endl;
	}
};

void GamePlayComputerStarts(void)
{
	char userInput = '~';
	string curWord = "";
	bool bGameEnded = false;
	string computerTargetWord;

	while(!bGameEnded)
	{
		char computerPlay = GetNextChar(curWord, computerTargetWord);
		putchar(computerPlay);
		putchar(' ');
		if(computerPlay == '?')
		{
			cout << endl << endl << "I am ready to give up. if you have a valid word in mind, you will win. But if you are bluffing, I will win." << endl;
			cout << "What word do you have in mind? (Use lower case alphabets only)" << endl;
			string usertargetword;
			getline( cin, usertargetword);
			if(usertargetword.length() > 20)
			{
				cout << endl << endl << "Sorry, that word you had in mind is too long! This game allows up to 20 characters per word. YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
			else if(usertargetword.compare(0, curWord.length(), curWord) != 0)
			{
				cout << endl << endl << "Sorry, \"" << usertargetword << "\" does not start with \"" << curWord << "\". Where's your mind? YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
			else if(aw.IsValidWord(usertargetword))
			{
				cout << endl << endl << "Aha! Clever! \"" << usertargetword << "\" is a valid word, did not see that coming! YOU WIN!!!" << endl;
				bGameEnded = true;
				break;
			}
			else
			{
				cout << endl << endl << "Sorry, \"" << usertargetword << "\" is not a valid word. YOU LOSE!" << endl;
				bGameEnded = true;
				break;
			}
		}
		curWord += computerPlay;
		if(aw.IsValidWord(curWord))
		{
			cout << endl << endl << "Oops. I completed \"" << curWord << "\", which is a valid word. YOU WIN! I lose" << endl << endl;
			bGameEnded = true;
			break;
		}
		else if(curWord.size() >= 20)
		{
			cout << endl << endl << "Too long. We will limit the number of letters to 20. GAME TIED" << endl << endl;
			bGameEnded = true;
			break;
		}

		userInput = getchar();
		putchar(' ');
		if(userInput < 97 || userInput > 122)
		{
			cout << endl << endl << "Only lowercase letters are allowed. YOU LOSE!!!!" << endl;
			bGameEnded = true;
			break;
		}

		curWord += userInput;
		if(aw.IsValidWord(curWord))
		{
			cout << endl << endl << curWord << " is a valid word, and guess who completed it? I win - YOU LOSE!" << endl << endl;
			bGameEnded = true;
			break;
		}
		else if(curWord.size() >= 20)
		{
			cout << endl << endl << "Too long. We will limit the number of letters to 20. GAME TIED" << endl << endl;
			bGameEnded = true;
			break;
		}
	}
	if(KBPlays.size() > 0)
	{
		cout << endl << endl << "The words I had in mind when I played are: " << endl;
		for(int k = 0; k < KBPlays.size(); k++)
		{
			cout << KBPlays[k] << endl;
		}
		cout << endl;
	}
};

int main()
{
	//bool bQuit = false;
	try
	{
		//aw.debug_printCount();
		//while(true)
		//{
			char cur_input = '\n';
			cout << endl << endl << "Welcome to wordtrain. This game is currently written for one human to play with me." << endl << endl;
			cout << "Do you want to start or do you want me to start? Type your response:" << endl << endl;
			cout << "1 - You start" << endl;
			cout << "2 - I start" << endl;
			cout << "q - Quit" << endl << endl;

			static struct termios oldt, newt;
			tcgetattr(STDIN_FILENO, &oldt);
			newt = oldt;
			newt.c_lflag &= ~(ICANON);
			tcsetattr(STDIN_FILENO, TCSANOW, &newt);

			while(cur_input == '\n')
			{
				cur_input = getchar();
				if(cur_input == '1')
				{
					cout << endl << "Well then. You start. Please use lower case characters only." << endl;
					cout << "Remember - whoever completes a valid word LOSES! The goal? Force your opponent to complete a valid word." << endl;
					cout << "Also, minimum length is 5. So if you force your opponent to play \"star\", game is NOT OVER YET." << endl;
					cout << "However, if the next player plays a 't' after \"star\", that player loses, as \"start\" is a valid word." << endl;
					cout << "Ready set go!" << endl << endl;
					GamePlayUserStarts();
				}
				else if(cur_input == 'q' || cur_input == 'Q')
				{
					cout << endl << "Goodbye" << endl;
					//bQuit = true;
				}
				else if(cur_input == '2')
				{
					cout << endl << "Well then. I will start. When it is your turn please use lowercase characters only." << endl << endl;
					cout << "Remember - whoever completes a valid word LOSES! The goal? Force your opponent to complete a valid word." << endl;
					cout << "Also, minimum length is 5. So if you force your opponent to play \"star\", game is NOT OVER YET." << endl;
					cout << "However, if the next player plays a 't' after \"star\", that player loses, as \"start\" is a valid word." << endl;
					cout << "Ready set go!" << endl << endl;
					GamePlayComputerStarts();
				}
				else
				{
					cout << endl << "Invalid response. Please use 1 if you want to start gameplay. ";
					cout << "Use 2 if you want me to start. To quit, type Q/q" << endl << endl;
					cur_input = '\n';
				}
			}

			tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
			cout << endl;
			//if(bQuit)
			//	break;
			//else
			//	KBPlays.clear();
		//}
	}
	catch(exception& e)
	{
		cout << "Exception caught in main() - " << e.what() << endl;
		return 1;
	}
	return 0;
}
