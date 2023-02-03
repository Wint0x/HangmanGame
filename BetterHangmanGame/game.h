#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <csignal>
#include <algorithm>
#include <unordered_map>
#include <sstream>

#include <fstream>

typedef std::vector<std::string> wordlist;

void readWords(wordlist&);
std::string prepare_word(std::string word, std::unordered_map<int,char>&);
std::ostringstream joinStrings(wordlist&, const std::string&);

void signal_handler(int sig_num)
{
	if (sig_num == 22)
	{
		std::cerr << "Couldn't open the \"wordlist\" file for read! Program closed with code [" << sig_num << "]\n" << std::flush;
		exit(22);
	}

	if (sig_num == 15)
	{
		std::cout << "Bye" << std::endl;
		exit(15);
	}

	std::cerr << "Program closed with code [" << sig_num << "\n" << std::flush;
	exit(sig_num);
}