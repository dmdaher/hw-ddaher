/*
 * Dictionary.h
 *
 *  Created on: Sep 18, 2016
 *      Author: kempe
 */

// The following is a suggestion for how to proceed.
// Feel free to make any changes you like.

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <string>
#include <set>

class Dictionary {
public:
	Dictionary (std::string dictionary_file_name);
	bool find(std::string wordSearch);
private:
	std::set <std::string> mDictionaryset;
};


#endif /* DICTIONARY_H_ */
