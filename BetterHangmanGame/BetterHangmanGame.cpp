#include "game.h"

using std::cout, std::cin, std::endl, std::getline, std::ws, std::string, std::vector;

//Open file, fill vector with all words
void readWords(wordlist& words)
{
	const string path = "words.txt";
	std::ifstream read(path);

	if (!read.is_open())
	{
		std::raise(SIGABRT);
	}

	string line;
	while (!read.eof())
	{
		read >> line;
		words.push_back(line);
	}
}

std::string prepare_word(std::string word, std::unordered_map<int, char>& original_word_map)
{
	size_t len = word.length();
	string new_word{};

	for (auto& letter : word)
	{
		new_word += "_ ";
	}

	//Assign char at correct index
	short original_index = 0;
	for (short index = 0; index < new_word.length() - 1; ++index)
	{
		if (new_word[index] == '_')
		{
			original_word_map[index] = word[original_index];
			++original_index;
		}
	}

	//No initial letter display
	if (len <= 4)
	{
		return new_word;
	}

	//Show only initial letter
	else if (len >= 5 && len <= 8)
	{
		new_word[0] = word[0];

		original_word_map.erase(original_word_map.begin()); //Remove first pair
		return new_word;
	}

	//Show initial and last letter
	else
	{
		new_word[0] = word[0];
		new_word[new_word.length() - 2] = word[len - 1]; //-2 because last is a space
		
		original_word_map.erase(original_word_map.begin()); //Remove first and last pair
		original_word_map.erase(--original_word_map.end());
		return new_word;
	}
}

std::ostringstream Join(wordlist& words, const std::string& delimiter)
{
	std::ostringstream oss;

	for (size_t i = 0; i < words.size(); i++)
	{
		oss << words[i];
		/*
		Keeps adding the delimiter to the stream until we reach the
		last element of the vector.
		(We don't want to add a delimiter after the last element).
		*/
		if (i != words.size() - 1)
		{
			oss << delimiter;
		}
	}
	return oss;
}

static const string MENU = "1) Play.\n2) View guessed words.\n3) Exit.\n--> ";

int main()
{
	std::signal(SIGABRT, signal_handler);
	std::signal(SIGTERM, signal_handler);

	//Generate random device
	std::random_device rd;
	std::mt19937 mt(rd());

	wordlist words{}; //Our word-list
	readWords(words); //Gets words from file and fill vector

	cout << "Loaded " << words.size() << " words!\n";

	//Randomly shuffle wordlist and erase most words (We dont need all 300k, xD)
	std::shuffle(words.begin(), words.end(), mt);
	words.erase(words.begin(), words.end() - words.size() / 200);

	cout << "Picked " << words.size() << " random words!\n\n";

	//Random int distribution
	std::uniform_int_distribution<> distr(0, words.size() - 1);

	//Game variables
	string option{}; //For Menu choice
	int points{ 0 }, tries{ 0 }; //Guessed words so far
	string to_guess{}, prepared{}; //Word to guess holder
	wordlist words_that_got_picked{}, words_you_guessed{}; //Words that have been picked already
	std::unordered_map<int, char> original_word_map{};

	string letter{}; //Letter to guess
	wordlist unavailable_letters{};
	bool isWord = false, letterFound = false; //false letter, true word

	const string delimiter = ", ";

	//Game start
	while (true)
	{
		//cout << "\nYou guessed: " << wins << " words so far!\n\n";
		cout << MENU;
		getline(cin >> ws, option);

		//Game start
		if (option == "1")
		{
			tries = 0;
			unavailable_letters.clear();
			original_word_map.clear();

			//Pick random word
			to_guess = words[distr(mt)];

			words.erase(std::remove(words.begin(), words.end(), to_guess), words.end()); //Remove from vector
			words_that_got_picked.push_back(to_guess);

			//Prepare word here
			prepared = prepare_word(to_guess, original_word_map);

			//Loop for guess, either enter a full word or a letter
			while (letter != to_guess && tries < 8)
			{
				letterFound = false;
				isWord = false;

				cout << "\nTries left: " << 8 - tries << "\n\n";

				cout << prepared << endl;
				//cout << to_guess << endl;
				cout << "Guess letter or word: ";
				getline(cin >> ws, letter);

				//Did user guess a letter or full word?
				if (letter.length() > 1)
				{
					isWord = true;
				}

				//Try full word guess
				if (isWord)
				{
					//Guessed
					if (letter == to_guess)
					{
						//Probably call a win function
						cout << "\nYou guessed the word! The word was: " + to_guess << "\n";
						cout << "Took you: " << tries << ((tries > 1 || tries == 0 ? " tries" : " try")) << "\n\n";
						//unavailable_letters.clear();
						words_you_guessed.push_back(to_guess);
						break;
					}

					else 
					{
						cout << "\nYou did not guess the word!\n\n";
						tries++;
						continue;
					}
				}

				//Letter in word? Check if it exists in the map
				for (auto& [index, character] : original_word_map)
				{
					if (letter[0] == character)
					{
						prepared[index] = letter[0];
						letterFound = true;

					}
				}

				//Didn't found a letter in the map, increase tries
				if (!letterFound)
				{
					tries++;
				}

				//If there are no more "_" inside the string, we guessed the word!
				if (prepared.find('_') == string::npos)
				{
					cout << "\nYou guessed the word! The word was: " + to_guess << "\n";
					cout << "Took you: " << tries << ((tries > 0 ? " tries" : " try")) << "\n\n";
					//unavailable_letters.clear();					

					words_you_guessed.push_back(to_guess);

					break;
				}

				//Already guessed this letter?
				if (std::count(unavailable_letters.begin(), unavailable_letters.end(), letter) >= 1)
				{
					cout << "\nYou already guessed this letter!\n\n";
					
				}

				//Letter not in list, add it
				else
				{
					if (!isWord) unavailable_letters.push_back(letter);
				}

				//Display letters that the user entered here
				if (!unavailable_letters.empty()) cout << "\nPicked: " << Join(unavailable_letters, delimiter).str() << endl;

			}

			//You lose xD
			if (tries == 8)
			{
				cout << "\nOut of tries! GAME OVER!!!\nThe word was: " << to_guess << "\n\n" << std::flush;
			}

		}
		//Guessed words
		else if (option == "2")
		{
			if (words_you_guessed.empty())
			{
				cout << "\nYou guessed 0 words so far!\n\n";
			}

			else
			{
				cout << "You guessed " << words_you_guessed.size() << ((words_you_guessed.size() > 1) ? " words" : " word") << " so far!\n\n";
				cout << "This are the words you guessed:\n"; 
				cout << Join(words_you_guessed, delimiter).str() << "\n\n";
			}

			if (words_that_got_picked.empty())
			{
				cout << "You played 0 games so far!\n\n";
			}

			else
			{
				cout << "This are all the words that got picked so far:\n";
				cout << Join(words_that_got_picked, delimiter).str() << "\n" << endl;
			}
		}
		//Quit
		else if (option == "3")
		{
			std::raise(SIGTERM); //Termination signal
		}

		else
		{
			std::cerr << "\nUnrecognized option, try again!\n\n";
		}
	}

	return 0;
}
