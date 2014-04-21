/*
 *  bwtSearch.cpp :
 *  A simple search program that implements BWT backward search, which can
 *      efficiently search a BWT encoded file.
 *  The program also has the capability to decode the BWT encoded file 
 *      back to its original file in a lossless manner.
 *
 *  Created on: Apr 10, 2013
 *      Author: Vu Quang Hoa - z3402013 - UNSW
 */

#include <string.h>
#include <iostream>
#include <map>
#include <utility>
#include <fstream>

#define CH_MAX 256			//The maximum number of characters
#define BLOCK_MAX 100000	//The maximum number of characters in one block
#define RESULT_MAX 1000		//The maximum number of result strings

using namespace std;

/*
 * Function: Decode backward
 * Find the character before the given one in the result string
 */
unsigned long int getOrderOfNextChInFirstArray(unsigned long int order, ifstream &myReadFile, unsigned long int last_block, unsigned long int CH_function[CH_MAX]);
/*
 * Function: Decode forward
 * Find the character before the given one in the result string
 */
unsigned long int getOrderOfNextChInLastArray(unsigned long int order, ifstream &myReadFile, unsigned long int last_block, unsigned long int CH_function[CH_MAX]);
/*
 * Function: Write map of characters occurrence in blocks to INDEX file
 */
bool writeToIndexFile(const char *fileName, unsigned long int block_index);
/*
 * Function: Read map of characters occurrence in blocks from INDEX file
 */
bool readFromIndexFileToMap(const char *indexFile, unsigned long int *block_index);
/*
 * Function: Create map from BWT file
 */
bool createMapFromBwtFile(const char *bwtFile, unsigned long int *block_index);
/*
 * Functions: partition and quick sort to sort unique identifier number of result strings
 */
int partition(unsigned long int* input, int p, int r);
void quicksort(unsigned long int* input, int p, int r);
/*
 * Function: get the size of file
 */
unsigned long int fileSize(const char* fileName);
/*
 * Function: convert string to long int number
 */
unsigned long int convertStringToLongInt(string str);
/*
 * Function: pre processing
 * Pre process file index and file bwt to get BLOCK_INDEX, and OCCUR array
 */
unsigned long int preProcessing(char* indexFile, char* bwtFile);
/*
 *  Function find first and last occurences of the search_string
 */
bool findFirstAndLastOccurences(unsigned long int *first, unsigned long int *last);
/*
 * Function find the result string based on the first and the last occurences of search_string
 */
void findResultStringBasedOnFirstAndLastOccurences(unsigned long int first, unsigned long int last, unsigned long int last_block_index);

//List of global parametters
map <unsigned long int, map<char, unsigned long int> > occur;		//Occurrence function
unsigned long int CH_function[CH_MAX] = {0};    //Use occurrences function to calculate CH_function
char *bwtFile;	//BWT file
char *indexFile;	//INDEX file
char *search_str;	//SEARCH string
map<int, string> result_array;  //Array of result strings
unsigned long int id_array[RESULT_MAX] = {0};	//Identifiers array
int result_index = 0;   //Number of result

/*
 * Main function
 */
int main(int argc, char* argv[])
{
	unsigned long int block_index = 1;									//Number of blocks in map
	map<unsigned long int, map<char, unsigned long int> >::iterator it;
	map<char, unsigned long int>::iterator inner_it;

	//Check the number of input variables
	if (argc != 4) {
		cout << "Usage: btwsearch <fileName> <indexFile> <search_term> \n";
		return 0;
	}
	bwtFile = argv[1];	//BWT file
	indexFile = argv[2];	//INDEX file
	search_str = argv[3];	//SEARCH string

    //Preprocess file index and file bwtFile
	block_index = preProcessing(indexFile, bwtFile);

	//Use occurrences function to calculate CH_function
	unsigned long int last_block_index = block_index;
	for (int i = 0; i < CH_MAX; i++) {
		if (occur[last_block_index][(char)i] > 0) {
			unsigned long int sum = 0;
			for (int j = 0; j < i; j++) {
				if (occur[last_block_index][(char)j] > 0) {
					sum += occur[last_block_index][(char)j];
				}
			}
			CH_function[i] = sum + 1;
		}
	}

	// Start to find the search_string in the file
	unsigned long int index = strlen(search_str) - 1;
	char ch = search_str[index];	//The last character of search_string
	//If the last block does not contain any character ch -> return 0
	if (occur[last_block_index][ch] == 0)
		return 0;

	unsigned long int first = CH_function[(int)ch];
	unsigned long int last = occur[last_block_index][ch] + CH_function[(int)ch] - 1;
    
    if (!findFirstAndLastOccurences(&first, &last))
        return 0;

	if (first > last)
		return 0;
    findResultStringBasedOnFirstAndLastOccurences(first, last, last_block_index);
    
	//Sort the identifiers
	quicksort(id_array, 0, result_index);
	//Print out the result
	for (int k = 0; k <= result_index; k++) {
		if (k > 0 && id_array[k-1] == id_array[k])
			continue;
		if (result_array[id_array[k]].length() > 0)
			cout << result_array[id_array[k]] << "\n";
	}
	return 1;
}

/*
 * Function find the result string based on the first and the last occurences of search_string
 */
void findResultStringBasedOnFirstAndLastOccurences(unsigned long int first, unsigned long int last, unsigned long int last_block_index) {
    unsigned long int index = strlen(search_str) - 1;
	char ch = search_str[index];
    for (unsigned long int i = first; i <= last; i++) {
		string result = "";
		unsigned long int temp_ch = 0;
		unsigned long int temp_i;
		int k = 0;
		temp_i = i;
		ifstream myReadFile1;
		myReadFile1.open(bwtFile, ios::in);
		//Decode backward
		do {
			unsigned long int order = getOrderOfNextChInFirstArray(temp_i, myReadFile1, last_block_index, CH_function);
			int j;
			for (j = 0; j < CH_MAX; j++) {
				if (CH_function[j] <= order && CH_function[j] > 0)
					temp_ch = j;
			}
			result.insert(0, 1, (char)temp_ch);
			temp_i = order;
		} while (temp_ch != '[');
		//Add the current character ch
		result += (char)ch;
		temp_i = i;
		//Decode forward
		do {
			unsigned long int order = getOrderOfNextChInLastArray(temp_i, myReadFile1, last_block_index, CH_function);
			int j;
			for (j = 0; j < CH_MAX; j++) {
				if (CH_function[j] <= order && CH_function[j] > 0)
					temp_ch = j;
				if (CH_function[j] > order)
					break;
			}
			if ((char)temp_ch != '[')
				result += (char)temp_ch;
			temp_i = order;
			k++;
		} while (temp_ch != '[');
        
		/*
		 * When we find the result string
		 * 	Put string in the result_array
		 * 	Put the identifier of string in id_array
		 */
		myReadFile1.close();
		size_t found = result.find(']');
		string tmp = result.substr(1, found - 1);
		unsigned long int nb = convertStringToLongInt(tmp);
		result = result.substr(found + 1, result.length());
		if (result_array[nb].length() == 0)
			result_array[nb] = result;
		id_array[result_index] = nb;
		result_index++;
	}
}

/*
 *  Function find first and last occurences of the search_string
 */
bool findFirstAndLastOccurences(unsigned long int *first, unsigned long int *last) {
    ifstream myReadFile;
	myReadFile.open(bwtFile, ios::in);
    
	/*
	 * Start a loop to find how many search_str in the file
	 * backward: from the last letter of search_str
	 */
    unsigned long int index = strlen(search_str) - 1;
    char ch = search_str[index];	//The last character of search_string
    
	while (first <= last && index > 0) {
		ch = search_str[index - 1];
		unsigned long int first_block = *first / BLOCK_MAX;
		unsigned long int first_remain = *first % BLOCK_MAX;
		if (first_remain==0) {
			first_remain = BLOCK_MAX * first_block--;
		}
		unsigned long int nb_of_ch_before_first_block = occur[first_block][ch];
		unsigned long int last_block = *last / BLOCK_MAX;
		unsigned long int last_remain = *last % BLOCK_MAX;
		unsigned long int i = 1;
		unsigned char temp_ch;
		unsigned long int nb_of_ch_before_first_remain = 0;
		unsigned long int nb_of_ch_from_first_to_last_remain = 0;
        
		//Find the next first and last rank of the character ch
		//If the first block equals to the last block -> all found ch characters in first_block
		if (first_block == last_block) {
			i = 1;
			myReadFile.seekg(first_block * BLOCK_MAX, ios_base::beg);
			//Find the number of ch from the beginning of the first_block to first_remain
			while (i < first_remain && !myReadFile.eof()) {
				myReadFile >> noskipws >> temp_ch;
				if (temp_ch == ch)
					nb_of_ch_before_first_remain++;
				i++;
			}
			//Find the number of ch from the beginning of the first_remain to last_remain
			while (i <= last_remain && !myReadFile.eof()) {
				myReadFile >> noskipws >> temp_ch;
				if (temp_ch == ch)
					nb_of_ch_from_first_to_last_remain++;
				i++;
			}
			if (nb_of_ch_from_first_to_last_remain == 0)
				return false;
            
			unsigned long int order_of_first_ch_from_first_to_last = nb_of_ch_before_first_block + nb_of_ch_before_first_remain;
			unsigned long int order_of_last_ch_from_first_to_last = order_of_first_ch_from_first_to_last + nb_of_ch_from_first_to_last_remain - 1;
			*first = CH_function[(int)ch] + order_of_first_ch_from_first_to_last;
			*last = CH_function[(int)ch] + order_of_last_ch_from_first_to_last;
		}
		else if (first_block <= last_block) {
			i = 1;
			unsigned long int nb_of_ch_from_first_to_last_block;
			nb_of_ch_from_first_to_last_block = occur[last_block][ch] - occur[first_block][ch];
			myReadFile.seekg(first_block * BLOCK_MAX, ios_base::beg);
			//Find the number of ch from the beginning of the first_block to first_remain
			while (i < first_remain && !myReadFile.eof()) {
				myReadFile >> noskipws >> temp_ch;
				if (temp_ch == ch)
					nb_of_ch_before_first_remain++;
				i++;
			}
			i = 1;
			myReadFile.seekg(last_block * BLOCK_MAX, ios_base::beg);
			//Find the number of ch from the beginning of the last_block to last_remain
			while (i <= last_remain && !myReadFile.eof()) { //Check if the last_remain contain ch or not
				myReadFile >> noskipws >> temp_ch;
				if (temp_ch == ch)
					nb_of_ch_from_first_to_last_remain++;
				i++;
			}
            
			unsigned long int order_of_ch_from_first_to_last;
			order_of_ch_from_first_to_last = nb_of_ch_from_first_to_last_block + nb_of_ch_from_first_to_last_remain
            - nb_of_ch_before_first_remain;
            
			if (order_of_ch_from_first_to_last == 0)
				return 0;
			unsigned long int order_of_first_ch_from_first_to_last = nb_of_ch_before_first_block + nb_of_ch_before_first_remain;
			unsigned long int order_of_last_ch_from_first_to_last = order_of_first_ch_from_first_to_last + order_of_ch_from_first_to_last - 1;
			*first = CH_function[(int)ch] + order_of_first_ch_from_first_to_last;
			*last = CH_function[(int)ch] + order_of_last_ch_from_first_to_last;
		}
		index--;
	}
    myReadFile.close();
	/* End of loop */
    return true;
}
/*
 * Function: pre processing
 * Pre process file index and file bwt to get BLOCK_INDEX, and OCCUR array
 */
unsigned long int preProcessing(char* indexFile, char* bwtFile) {
    unsigned long int block_index;
    ifstream myIndexFile;
	myIndexFile.open(indexFile, ios::in);
	//If the index file exists: the bwtFile is a large file, we only need to process the index file
	if (myIndexFile.good()) {
		myIndexFile.close();
		//Read index file to maps
		readFromIndexFileToMap(indexFile, &block_index);
	}
	//Otherwise
	else {
		//If the bwtFile is larger than ~1Mb, then create the index file.
		//Create map from bwtFile
		createMapFromBwtFile(bwtFile, &block_index);
		if (fileSize(bwtFile) > 10 * BLOCK_MAX) {
			//Create index file from map
			writeToIndexFile(indexFile, block_index);
		}
	}
    myIndexFile.close();
    return block_index;
}




/*
 * Function: Decode backward
 * Find the character before the given one in the result string
 */
unsigned long int getOrderOfNextChInFirstArray(unsigned long int order, ifstream& myReadFile, unsigned long int last_block,
		unsigned long int CH_function[CH_MAX]) {
	unsigned long int resultOrder = -1;
	myReadFile.seekg(order-1, ios_base::beg);
	char ch;
	myReadFile >> noskipws >> ch;
	unsigned long int order_block = order / BLOCK_MAX;
	unsigned long int order_remain = order % BLOCK_MAX;
	unsigned long int nb_of_ch_before_first_block = 0;
	if (order_block != 0) {
		nb_of_ch_before_first_block = occur[order_block][ch];
	}
	unsigned long int i = 0;
	char temp;
	myReadFile.seekg(order_block * BLOCK_MAX, ios_base::beg);
	while (!myReadFile.eof() && i < order_remain) {
		myReadFile >> noskipws >> temp;
		if (temp == ch)
			nb_of_ch_before_first_block++;
		i++;
	}
	resultOrder = nb_of_ch_before_first_block + CH_function[ch] - 1;
	return resultOrder;
}

/*
 * Function: Decode forward
 * Find the character before the given one in the result string
 */
unsigned long int getOrderOfNextChInLastArray(unsigned long int order, ifstream& myReadFile, unsigned long int last_block,
		unsigned long int CH_function[CH_MAX]) {
	unsigned long int resultOrder = 0;
	unsigned long int j;
	int current_ch;
	for (j = 0; j < CH_MAX; j++) {
		if (CH_function[j] <= order && CH_function[j] > 0)
			current_ch = j;
	}
	unsigned long int order_ch = order - CH_function[current_ch] + 1;
	int temp = 0;
	j = 0;
	for (j = 0; j < last_block; j++) {
		if (occur[j][current_ch] < order_ch && occur[j][current_ch] > 0)
			temp = j;
		if (occur[j][current_ch] > order_ch)
			break;
	}
	myReadFile.seekg(temp * BLOCK_MAX, ios_base::beg);
	unsigned char temp_ch;
	unsigned long int count_ch = 0;
	int k = 1;
	while (!myReadFile.eof()) {
		myReadFile >> noskipws >> temp_ch;
		if (temp_ch == current_ch)
			count_ch++;
		if (count_ch == order_ch - occur[temp][current_ch])
			break;
		k++;
	}
	resultOrder = k + temp * BLOCK_MAX;
	return resultOrder;
}


/*
 * Function: partition
 */
int partition(unsigned long int* input, int p, int r)
{
    int pivot = input[r];

    while ( p < r )
    {
        while ( input[p] < pivot )
            p++;

        while ( input[r] > pivot )
            r--;

        if ( input[p] == input[r] )
            p++;
        else if ( p < r )
        {
            int tmp = input[p];
            input[p] = input[r];
            input[r] = tmp;
        }
    }

    return r;
}

/*
 * Function: quick sort
 */
void quicksort(unsigned long int* input, int p, int r)
{
    if ( p < r )
    {
        int j = partition(input, p, r);
        quicksort(input, p, j-1);
        quicksort(input, j+1, r);
    }
}

/*
 * Function: Write map to INDEX file
 */
bool writeToIndexFile(const char *indexFile, unsigned long int block_index) {
	map<unsigned long int, map<char, unsigned long int> >::iterator it;
	map<char, unsigned long int>::iterator inner_it;

	ofstream myWriteFile;
	myWriteFile.open(indexFile);
	for (it=occur.begin(); it!=occur.end(); it++) {
		for(inner_it=(*it).second.begin(); inner_it != (*it).second.end(); inner_it++) {
			if ((*inner_it).second > 0) {
				myWriteFile << (*it).first << "\n";
				myWriteFile << (int)(*inner_it).first << "\n" << (*inner_it).second << "\n";
			}
		}
	}
	myWriteFile.close();
	return true;
}

/*
 * Function: Read map from INDEX file
 */
bool readFromIndexFileToMap(const char *indexFile, unsigned long int *block_index) {
	ifstream myReadFile;
	myReadFile.open(indexFile, ios::in);
	string line;
	unsigned long int block_nb = 1;
	int character;
	unsigned long int nb_of_ch;
	unsigned long int index = 0;
	if (myReadFile.is_open()) {
		while (getline(myReadFile, line)) {
			index++;
			if (index % 3 == 1)
				block_nb = convertStringToLongInt(line);
			else if (index % 3 == 2)
				character = convertStringToLongInt(line);
			else {
				nb_of_ch = convertStringToLongInt(line);
				index = 0;
				occur[block_nb++][(char)character] = nb_of_ch;
				*block_index = block_nb - 1;
			}
		}
	}
	myReadFile.close();
	return true;
}


/*
 * Function: Get the size of the file
 */
unsigned long int fileSize(const char* fileName) {
	ifstream f;
	f.open(fileName, std::ios_base::binary | std::ios_base::in);
	if (!f.good() || f.eof() || !f.is_open()) { return 0; }
	f.seekg(0, std::ios_base::beg);
	ifstream::pos_type begin_pos = f.tellg();
	f.seekg(0, std::ios_base::end);
	unsigned long int result = static_cast<int>(f.tellg() - begin_pos);
	return result;
}


/*
 * Function: Create index file based on map
 */
bool createMapFromBwtFile(const char *bwtFile, unsigned long int *block_index) {
	unsigned long int count[CH_MAX] = {0};
	unsigned long int block_nb = 1;
	ifstream myReadFile;
	myReadFile.open(bwtFile, ios::in);
	if (myReadFile.is_open()) {
		unsigned char ch;
		unsigned long int i = 0;
		while (!myReadFile.eof()) {
			if (i == BLOCK_MAX) {
				int j = 0;
				for (j = 0; j < CH_MAX; j++)
					occur[block_nb][(char)j] = count[j];
				block_nb++;
				i = 0;
			}
			else {
				myReadFile >> noskipws >> ch;
				if (myReadFile.eof())
					break;
				count[(int)ch]++;
				i++;
			}
		}
		for (int j = 0; j < CH_MAX; j++)
			occur[block_nb][(char)j] = count[j];
	}
	*block_index = block_nb;
	myReadFile.close();
	return true;
}

/*
 * Function: Convert string to long int
 */
unsigned long int convertStringToLongInt(string str) {
	unsigned long int number = 0;
	for (unsigned int i = 0; i < str.length(); i++) {
		number = number * 10 + str[i] - (int)'0';
	}
	return number;
}
