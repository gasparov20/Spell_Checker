#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include "QuadraticProbing.h"
using namespace std;

int spellcheck(string word, HashTable<string> & dictionary, ofstream &fout)
{
	int chain;
	char t = ' ';
	string temp;

	chain = 0;
	temp = word;

	// Print spelling suggestions assuming there's an extra character
	for (int i = 0; i < word.length(); i++)
	{
		word.erase(i, 1);

		if (dictionary.contains(word))
		{
			fout << word << " ";
			chain++;
		}

		word = temp;
	}

	// Print spelling suggestions assuming there's one missing character
	for (int i = 0; i <= word.length(); i++)
	{
		for (char c = 'a'; c <= 'z'; c++)
		{
			word.insert(i, 1, c);

			if (dictionary.contains(word))
			{
				fout << word << " ";
				chain++;
			}

			word = temp;
		}
	}

	// Print spelling suggestions assuming there was an adjacent transposition
	for (int i = 0; i < word.length() - 1; i++)
	{
		swap(word[i], word[i + 1]);

		if (dictionary.contains(word))
		{
			fout << word << " ";
			chain++;
		}

		word = temp;
	}

	// Print spelling suggestions assuming one character is incorrect
	for (int i = 0; i < word.length(); i++)
	{
		for (char c = 'a'; c <= 'z'; c++)
		{
			word[i] = c;

			if (dictionary.contains(word))
			{
				fout << word << " ";
				chain++;
			}

			word = temp;
		}
	}

	// Spelling suggestions complete, go to next line
	fout << endl;

	// Return the chain length for the word that was spellchecked
	return chain;
}

// Replaces any non-alphabetical char with a blank space
string removePunc(string line)
{
	for (int i = 0; i < line.length(); i++)
	{
		if (!isalpha(line[i]))
			line[i] = ' ';
	}
	return line;
}

int main()
{
	string dictName, inputName, data, lineStr, word = "", word2;
	int numWords, x = 0, lineNum = 1;
	vector<int> chains;
	ifstream finDic, finIn;
	ofstream fout;

	// Input prompts
	cout << "Dictionary file name: ";
	cin >> dictName;
	cout << "Number of words in dictionary: ";
	cin >> numWords;
	cout << "Input file name: ";
	cin >> inputName;

	// Create the hash table object for the dictionary.
	// The number of buckets is twice the size of the dictionary.
	HashTable<string> dictionary = HashTable <string>(2 * numWords);

	// Open the output file, the dictionary file, and the input file.
	fout.open("output.txt");
	finDic.open(dictName);
	finIn.open(inputName);

	// Populate the dictionary hash table.
	for (int i = 0; i < numWords; i++)
	{
		finDic >> data;
		dictionary.insert(data);
	}

	// Priming statement for input stream
	getline(finIn, lineStr);
	lineStr = removePunc(lineStr);

	while (!finIn.eof()) // Get each line in the file
	{
		// Loop through the string resulting from getline
		for (int i = 0; i < lineStr.length(); i++)
		{
			// If a character isn't a blank space, add it to the current word string
			if (lineStr[i] != ' ') 
				word += lineStr[i];

			// When a full word is read from the string from getline, check if it's in the dictionary.
			// If it's not in the dictionary, send it to the spell checker
			if ((word.length() > 0 && lineStr[i] == ' ') || (word.length() > 0 && i == lineStr.length() - 1))
			{
				if (!dictionary.contains(word))
				{
					word2 = word;

					for (int j = 0; j < word2.length(); j++)
						word2[j] = tolower(word2[j]);

					// If the word isn't found in the dictionary, check
					// its lower case counterpart just to be sure
					if (!dictionary.contains(word2))
					{
						// If the word isn't in the dictionary, print it and send it to the spellchecker
						fout << word << "(" << lineNum << "): ";
						x = spellcheck(word, dictionary, fout); // x is the chain length for each word
						if (x == 0)
							chains.push_back(1); // Chain length of 0 is 1 for computing average
						else
							chains.push_back(x); // Keeps track of chain length for each word
					}
				}
				word = ""; // Reset the variable so the next word can be processed
			}
		}
		// Going to the next line, so increment the line number,
		// get the next line, and remove the punctuation
		lineNum++;
		getline(finIn, lineStr);
		lineStr = removePunc(lineStr);
	}
	fout << endl;
	
	// Print the stats for the dictionary and input files used for this run to a file
	dictionary.stats(chains);
	chains.clear();
}