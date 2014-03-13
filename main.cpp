/*
 * Demand Paging Project
 *
 * A hybrid C/C++ program that simulates demand paging using six different
 * algorithms: FIFO, Least Recently Used, Most Recently Used, the theoretical
 * optimum algorithm, and then two algorithms that randomly generate numbers:
 * one that uses a uniformly distributed scheme of random numbers to calculate
 * frames that should be freed and one that uses the pseudorandom rand() function
 * supplied with C/C++ in order to calculate pages to remove.
 *
 * A hybrid of both languages was chosen due to the want for the speed and
 * preciseness that C offers with the added libraries and functions that C++
 * provides.
 *
 * @author: Marcos Davila
 * @date: 12/10/2012
 */

// The maximum amount of pages that this process will be using
#define MAX_NUM_PAGES 1024

// The total number of free frames the process has to work with
#define MAX_PAGE_FRAMES 48

// The number of processes a CPU will work on.
#define PROC_POOL_SIZE 500

// Valid / invalid bits signifying if an entry in a table points to
// a valid memory address.
#define INVALID_BIT 0
#define VALID_BIT 1

#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctype.h>
#include <cstring>

using std::string;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::transform;

int isInMemory(int frame_table[MAX_PAGE_FRAMES][2], int page, int reference);
string displayReferenceString();
void displayPageTable(int page_table[MAX_NUM_PAGES][3], string type);
void RAN(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string ref);
void OPT(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string ref);
int identifyPageToRemove(int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list);
void RU(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string type);
void FIFO(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list);
void createReferenceString();

// Global variables which keep track of user's preferences for output for all
// page replacement algorithms
int enableVerboseOutput = 0;
string vb = "";

int main(){
	/*
	 * Seed the random number generator and create the addresses for the process.
	 * The addresses reference what parts of the program should be paged in and
	 * out
	 */
	srand(time(NULL));
	createReferenceString();

	/* A page table is needed to store the mapping between virtual addresses
	 * and physical addresses. For this process, the maximum number of entries
	 * in the page table is LAS_SIZE / PAGE_SIZE, or 2^23 / 2^3 = 2^20.
	 * This should be reflected in the variable MAX_NUM_PAGES.
	 *
	 * The first entry contains the base address of each page in physical
	 * memory. The second entry contains whether this offset is valid. On
	 * algorithms that require it, the third position is auxiliary.
	 */
	int page_table[MAX_NUM_PAGES][3];

	/*
	 * A free frame list is necessary for knowing what frames in physical memory
	 * are free so proper frames can be allocated as necessary. At the beginning,
	 * all pages are available due to our use of demand paging.
	 */
	 vector<int> free_frame_list(MAX_PAGE_FRAMES);

	/*
	 * A frame table is required to keep track of the allocation details of
	 * physical memory. It keeps track of which frames are allocated, which
	 * frames are available, how many total frames there are, and so on.
	 *
	 * The frame table has one entry for each physical page frame, indicating
	 * whether the latter is free or allocated and, if it is allocated, to
	 * which page of this process.
	 *
	 * The first column in the frame table designates what page is occupying
	 * that entry (-1 is for entries that are not occupied). On
	 * algorithms that require it, the second position is auxiliary.zz
	 */
	 int frame_table[MAX_PAGE_FRAMES][2];

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

	 /*
	  * Include the option for the user to print out the reference string and
	  * the page table after every fault.
	  */
	string vb;
	cout << "Do you want to enable verbose output? (Y/N)" << endl;
	cin >> vb;

	transform(vb.begin(), vb.end(), vb.begin(), tolower);

	if (vb == "y" || vb == "yes"){
		enableVerboseOutput = 1;
	}

	// Create the a string to hold the number of page references and
	// if desired by the user, print them to the screen
	string ref = displayReferenceString();

	/*
	 * Begin running simulations, starting with FIFO, then LRU, then
	 * MRU, then OPT, then RAN, then RAN2
	 */
	FIFO(page_table, frame_table, free_frame_list);

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

	RU(page_table, frame_table, free_frame_list, "LRU");

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

	RU(page_table, frame_table, free_frame_list, "MRU");

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

	// Run the optimal page replacement algorithm as a benchmark
	OPT(page_table, frame_table, free_frame_list, ref);

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

	// Run a random page replacement algorithm to determine its effectiveness
	// as compared to tried and true algorithms. This random replacement algorithm
	// first tries the uniformly distributed method of random number generation
	// and page replacement first
	RAN(page_table, frame_table, free_frame_list, "RAN");

	/*
	 * Set the initial data for all of the tables that each algorithm will
	 * use. The page_table and frame_table should be initialized to invalid,
	 * the free frame list should have all the possible frames (from 0 to
	 * MAX_PAGE_FRAMES) listed in its vector.
	 */
	for (int i = 0; i < MAX_NUM_PAGES; i++){
		page_table[i][0] = INVALID_BIT;
		page_table[i][1] = INVALID_BIT;
		page_table[i][2] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		frame_table[i][0] = 0;
		frame_table[i][1] = INVALID_BIT;
	}

	for (int i = 0; i < MAX_PAGE_FRAMES; i++){
		free_frame_list.push_back(i);
	}

    // This random replacement algorithm tries the pseudorandom method of
	// random number generation and page replacement
	RAN(page_table, frame_table, free_frame_list, "RAN2");

	return 0;
}

/*
 * Checks to see if the reference held in the frame table matches the
 * new reference. Useful for seeing if a page is already in memory or
 * if a different page needs to be replaced.
 */
int isInMemory(int frame_table[MAX_PAGE_FRAMES][2], int page, int reference){
	if (frame_table[page][0] == reference){
		return 1;
	} else {
		return 0;
	}
}

/*
 * Displays the reference string in row order on the console to the user
 */
string displayReferenceString(){
    string pageList;
    string temp;

	ifstream addresses;
	addresses.open("reference_string.txt");

	 if (addresses.is_open()) {
        while (!addresses.eof()) {
            addresses >> temp;
            pageList += temp + "\t";
        }
	 }
	 addresses.close();

	 if (enableVerboseOutput){
		cout << "Reference strings (in row order):\n" << pageList << endl;
	 }

	 return pageList;
}

/*
 * Display the page table after each page fault if it is requested by the user.
 * Depending on the algorithm, the output will be different.
 */
void displayPageTable(int page_table[MAX_NUM_PAGES][3], string type){
	cout << "Page Replacement Algorithm: " << type << endl;
	cout << "Page\t" << "Valid/Invalid Bit\t" << "Auxiliary\t" << endl;

	if (type == "FIFO" || type == "RAN" || type == "OPT") {
		for (int i = 0; i < MAX_NUM_PAGES; i++){
			cout << page_table[i][0] << "\t" << page_table[i][1] << endl;
		}
	} else {
		for (int i = 0; i < MAX_NUM_PAGES; i++){
			cout << page_table[i][0] << "\t" << page_table[i][1] << "\t" << page_table[i][2] << endl;
		}
	}
}

/*
 * Implement a randomized version of the page replacement algorithm to gauge how effective a randomized algorithm
 * would be in regards to page replacement. This algorithm uses both pseudorandom numbers as well as a uniformly
 * distributed random number generation scheme to compare the effectiveness of both types of generations to each
 * other as well as to the other page replacement methods.
 */
void RAN(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string ref){
    // Start by reading in addresses that the program would access in sequential order from the file
	ifstream addresses;
	addresses.open("reference_string.txt");

	string referenceString;

	int reference = 0;
	int fault_rate = 0;

	/*
	 * Continue reading from the file only if the file is open and not at the end of its input
	 */
	if (addresses.is_open()){
		while (!addresses.eof()) {
			addresses >> referenceString;
			reference = atoi(referenceString.c_str());

			// If the reference to physical memory is invalid, we need to get a
			// new page. Consult the free frame list to find a free frame on
			// the backing store
			if (page_table[reference][1] == INVALID_BIT){
				int freeframe = 0;

				// If we have no more free frames to allocate, then all of the
				// slots in the frame table must be occupied.
				if (free_frame_list.size() == 0){
					// No free frames are available, so we must generate a random
					// page to replace using a scheme based on ref
					if (ref == "RAN"){
                        freeframe = (double) rand() / (RAND_MAX+1.0) * (MAX_PAGE_FRAMES);
					} else {
                        freeframe = rand() % MAX_PAGE_FRAMES;
					}
				} else {
					// Pick a free frame from the top of the list
					freeframe = free_frame_list.back();

					// Remove this page from the free frame list and decrease the size of the
					// list by one.
					free_frame_list.pop_back();
				}

				// Update the tables
				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;

				frame_table[freeframe][0] = reference;
				fault_rate++;

				// Check to see if the user desires output
				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, ref);
					}
				}
			} else if (!isInMemory(frame_table, page_table[reference][0], reference)){
				// No free frames are available, so we must generate a random
                // page to replace
                int freeframe = 0;
				if (ref == "RAN"){
                        freeframe = (double) rand() / (RAND_MAX+1.0) * (MAX_PAGE_FRAMES);
					} else {
                        freeframe = rand() % MAX_PAGE_FRAMES;
					}

				// Mark the old page in the page table as invalid, place the new
				// page into its location in the page table and then update
				// both tables
				int oldframe = page_table[reference][0];
				int oldpage = frame_table[freeframe][0];

				page_table[oldpage][1] = INVALID_BIT;

				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;
				frame_table[freeframe][0] = reference;

				// Since we made one frame available, we must push it back onto the list
				free_frame_list.push_back(oldframe);
				fault_rate++;

				// Check to see if the user desires output
				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, ref);
					}
				}
			}
		}
	}

	// Display the RAN fault rate
	cout << ref << ": " << fault_rate << endl;
	addresses.close();
}


/*
 * Implement the optimal page replacement algorithm by first examining the input string of reference addresses and
 * constructing a table by which we would be able to infer the optimal page to replace at each point in the program.
 * Then, run the algorithm again on the same reference addresses using this table as a guide to determine what pages
 * to replace.
 */

void OPT(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string ref){
	// Start by reading in addresses that the program would access in sequential order from the file
	ifstream addresses, oracle;
	oracle.open("reference_string.txt");

	string referenceString;
	// Fill a vector with the reference strings so that they can be referenced later
	// when we are trying to predict pages to remove
	vector<int> reference_string;
	for (int i = 0; i < PROC_POOL_SIZE; i++){
		oracle >> referenceString;
		reference_string.push_back(atoi(referenceString.c_str()));
	}

	oracle.close();
	addresses.open("reference_string.txt");

	int reference = 0;
	int fault_rate = 0;
	int loops = 0;

	/*
	 * Continue reading from the file only if the file is open and not at the end of its input
	 */
	if (addresses.is_open()){
		while (!addresses.eof()) {
			addresses >> referenceString;
			reference = atoi(referenceString.c_str());

			if (page_table[reference][1] == INVALID_BIT){
				int freeframe = 0;

				// If we have no more free frames to allocate, then all of the
				// slots in the frame table must be occupied.
				if (free_frame_list.size() == 0){
					freeframe = identifyPageToRemove(frame_table, reference_string);
				} else {
					// Pick a free frame from the back of the list
					freeframe = free_frame_list.back();

					// Remove this page from the free frame list and decrease the size of the
					// list by one.
					free_frame_list.pop_back();
				}

				// Update the tables
				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;

				frame_table[freeframe][0] = reference;
				fault_rate++;

				// Check if the user desires output
				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, "OPT");
					}
				}

			} else if (!isInMemory(frame_table, page_table[reference][0], reference)){
				int freeframe = identifyPageToRemove(frame_table, reference_string);

				// Place the reference to this frame into the page table
				int oldframe = page_table[reference][0];
				int oldpage = frame_table[freeframe][0];

				page_table[oldpage][1] = INVALID_BIT;

				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;
				frame_table[freeframe][0] = reference;

				// Push the newly available frame back onto the vector
				free_frame_list.push_back(oldframe);
				fault_rate++;

				// Check if the user wants output
				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, "OPT");
					}
				}
			}

			// Remove the first element in the vector as long as there is one. There
			// will only be one case where there are no more elements left in the vector
			// but the method should summarily exit once this is found by having discovered
			// that there is no more input left in the file.
			if (reference_string.size() > 0){
				reference_string.erase(reference_string.begin());
			}

		}
	}

	// Display the OPT fault rate
	cout << "OPT : " << fault_rate << endl;
	addresses.close();
}

/*
 * A method used solely by the optimal page replacement algorithm. Given a reference string
 * and a frame table, the reference string and the frame table is compared to check which
 * currently occupied frame in the frame table will be used the farthest in time from the
 * current element. Said element will then be chosen for removal
 */
int identifyPageToRemove(int frame_table[MAX_PAGE_FRAMES][2], vector<int> reference_string){
    int victim = 0;

	// No free frames are available, so we must use the page reference string
	// to identify which page is going to be used last and replace it
	int futureref[MAX_PAGE_FRAMES];

	// All elements should be initialized to a positive invalid number. That way,
	// if it is never accessed again then we can tell because it will have a high
	// (essentially infinite) number of moves that it will require to get there
	for (int i = 0; i < MAX_PAGE_FRAMES; ++i){
		futureref[i] = PROC_POOL_SIZE + 100;
	}

	// Given this element and all elements in the frame table,
	// determine if any element in the frame table is referenced
	// again. If it is, place the value of turns that it will take to
	// get there into the array
	for (int i = 0; i < MAX_PAGE_FRAMES; ++i){
		for (int j = 0; j < reference_string.size(); j++){
			if (reference_string.at(j) == frame_table[i][0]){
				futureref[i] = j;
				break;
			}
		}
	}

	// Find the value with the largest amount of turns that it will take to
	// get there and use that as the frame that should be freed
	int max_turns = -1;

	for (int i = 0; i < MAX_PAGE_FRAMES; ++i){
		if (futureref[i] > max_turns){
			max_turns = futureref[i];
            victim = i;
		}
	}

	return victim;
}

/*
 * A method that implements both the MRU and LRU page replacement algorithms, depending on the string parameter type.
 *
 * LRU:
 * Implement the least recently used page replacement algorithm. There is a bit that keeps track of how many pages have
 * been accessed sicne any particular page was last accessed. The algorithm decides the least recently used page by detecting
 * the largest number out of all of these bits in that column of the array.
 *
 * MRU:
 * Implement the most recently used page replacement algorithm. There is a bit that keeps track of how many pages have
 * been accessed since any particular page was last accessed. The algorithm decides the most recently used page by detecting
 * the smallest number out of all of these bits in that column of the array.
 */
void RU(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list, string type){
	// Start by reading in addresses that the program would access in sequential order from the file
	ifstream addresses;
	addresses.open("reference_string.txt");

	string referenceString;

	int reference = 0;
	int fault_rate = 0;

	/*
	 * Continue reading from the file only if the file is open and not at the end of its input
	 */
	if (addresses.is_open()){
		while (!addresses.eof()) {
			addresses >> referenceString;
			reference = atoi(referenceString.c_str());
			// If the reference to physical memory is invalid, we need to get a
			// new page. Consult the free frame list to find a free frame on
			// the backing store
			if (page_table[reference][1] == INVALID_BIT){
				int freeframe = 0;
				fault_rate++;

				// If we have no more free frames to allocate, then all of the
				// slots in the frame table must be occupied.
				if (free_frame_list.size() == 0){
					// No free frames are available, so we must degrade to RU method of
					// selecting a frame. Decide upon what method to use based on parameter
					// type

					// No free frames are available, so we must degrade to MRU method of
					// selecting a frame. Select a frame whose RU bit is greatest.
					if (type == "MRU"){
						int k = 0;
						int max = frame_table[k][1];

						for (k = 1; k < MAX_PAGE_FRAMES; ++k){
							if (frame_table[k][1] > max){
								max = frame_table[k][1];
								freeframe = k;
							}
						}
					}
					// Default to LRU if input is invalid
					else {
						// No free frames are available, so we must degrade to LRU method of
						// selecting a frame. Select a frame whose the largest RU bit.
						int k = 0;
						int min = frame_table[k][1];

						for (k = 1; k < MAX_PAGE_FRAMES; ++k){
							if (frame_table[k][1] < min){
								min = frame_table[k][1];
								freeframe = k;
							}
						}
					}
				} else {
					// Pick a free frame from the top of the list
					freeframe = free_frame_list.back();

					// Remove this page from the free frame list and decrease the size of the
					// list by one.
					free_frame_list.pop_back();
				}

				// Update the tables
				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;

				frame_table[freeframe][0] = reference;

				// Increment all the other bits to indicate how many turns have gone by without
				// each of those pages being accessed.
				int p;
				for (p = 0; p < MAX_PAGE_FRAMES; ++p){
					if (p == freeframe) continue;

					frame_table[p][1] = frame_table[p][1]++;
				}

				// Check if the user desires output
				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, type);
					}
				}
			} else if (!isInMemory(frame_table, page_table[reference][0], reference)){
				int freeframe = 0;
				++fault_rate;

				// No free frames are available, so we must degrade to MRU method of
				// selecting a frame. Select a frame whose RU bit is greatest.
				if (type == "MRU"){
					int k = 0;
					int max = frame_table[k][1];
					for (k = 1; k < MAX_PAGE_FRAMES; ++k){
						if (frame_table[k][1] > max){
							max = frame_table[k][1];
							freeframe = k;
						}
					}
				}
				// Default to LRU if input is invalid
				else {
					// No free frames are available, so we must degrade to LRU method of
					// selecting a frame. Select a frame whose the largest RU bit.
					int k = 0;
					int min = frame_table[k][1];

					for (k = 1; k < MAX_PAGE_FRAMES; ++k){
						if (frame_table[k][1] < min){
							min = frame_table[k][1];
							freeframe = k;
						}
					}
				}

				// Mark the old page in the page table as invalid, place the new
				// page into its location in the page table and then update
				// both tables
				int oldframe = page_table[reference][0];
				int oldpage = frame_table[freeframe][0];

				page_table[oldpage][1] = INVALID_BIT;

				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;
				frame_table[freeframe][0] = reference;

				free_frame_list.push_back(oldframe);

				// Increment all the other bits to indicate how many turns have gone by without
				// each of those pages being accessed.
				int p;
				for (p = 0; p < MAX_PAGE_FRAMES; ++p){
					if (p == freeframe) continue;

					frame_table[p][1] = frame_table[p][1]++;
				}

				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, type);
					}
				}

			}

			// Otherwise, this reference was valid so reset the bit for this reference
			// and increase the bit for all others
			frame_table[page_table[reference][0]][1] = 0;

			// Increment all the other bits to indicate how many turns have gone by without
			// each of those pages being accessed.
			int p;
			for (p = 0; p < MAX_PAGE_FRAMES; ++p){
				if (p == page_table[reference][0]) continue;

				frame_table[p][1] = frame_table[p][1]++;
			}
		}
	}

	// Display the fault rate
	cout << type << ": " << fault_rate << endl;
	addresses.close();
}

/*
 * Performs page replacement by replacing the page that's been resident longest in the page table
 */
void FIFO(int page_table[MAX_NUM_PAGES][3], int frame_table[MAX_PAGE_FRAMES][2], vector<int> free_frame_list){
	// Start by reading in addresses that the program would access in sequential order from the file
	ifstream addresses;
	addresses.open("reference_string.txt");

	string referenceString;

	int reference = 0;
	int fault_rate = 0;
	int FIFOSelection = MAX_PAGE_FRAMES - 1;

	/*
	 * Continue reading from the file only if the file is open and not at the end of its input
	 */
	if (addresses.is_open()){
		while (!addresses.eof()) {
			addresses >> referenceString;
			reference = atoi(referenceString.c_str());

			// If the reference to physical memory is invalid, we need to get a
			// new page. Consult the free frame list to find a free frame on
			// the backing store
			if (page_table[reference][1] == INVALID_BIT){
				int freeframe = 0;

				// If we have no more free frames to allocate, then all of the
				// slots in the frame table must be occupied.
				if (free_frame_list.size() == 0){
					// No free frames are available, so we must degrade to FIFO method of
					// selecting a frame.
					freeframe = FIFOSelection;
					FIFOSelection--;

					// If the global variable to denote the least recent frame to be
					// placed into memory exceeds the number of pages in the page frame
					// table, roll over to 0 and start from the top again
					if (FIFOSelection == 0){
						FIFOSelection = MAX_PAGE_FRAMES - 1;
					}
				} else {
					// Pick a free frame from the back of the list
					freeframe = free_frame_list.back();

					// Remove this page from the free frame list and decrease the size of the
					// list by one.
					free_frame_list.pop_back();
				}

				// Mark the old page in the page table as invalid, place the new
				// page into its location in the page table and then update
				// both tables
				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;

				frame_table[freeframe][0] = reference;
				fault_rate++;

				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, "FIFO");
					}
				}
			} else if (!isInMemory(frame_table, page_table[reference][0], reference)){
				// If the bit is valid, we must then check that the page that we want is currently
				// resident in memory. If it is not, then we must bring it in by picking a "victim"
				// page to replace
				int freeframe = FIFOSelection;
				FIFOSelection--;

				// If the global variable to denote the least recent frame to be
				// placed into memory exceeds the number of pages in the page frame
				// table, roll over to 0 and start from the top again
				if (FIFOSelection == 0){
					FIFOSelection = MAX_PAGE_FRAMES - 1;
				}

				int oldframe = page_table[reference][0];
				int oldpage = frame_table[freeframe][0];

				if (oldpage != -1){
					page_table[oldpage][1] = INVALID_BIT;
				}

				page_table[reference][0] = freeframe;
				page_table[reference][1] = VALID_BIT;
				frame_table[freeframe][0] = reference;

				frame_table[oldframe][0] = -1;
				fault_rate++;

				if (enableVerboseOutput && vb != "never"){
					cout << "Do you want to display the page table? (YES/NO/NEVER)" << endl;
					cin >> vb;

					transform(vb.begin(), vb.end(), vb.begin(), tolower);

					if (vb == "yes" || vb == "y"){
						displayPageTable(page_table, "FIFO");
					}
				}
			}
		}
	}

	// Display the FIFO fault rate
	cout << "FIFO :" << fault_rate << endl;
	addresses.close();
}

/*
 * Create a series of page reference strings that the process will access
 */
void createReferenceString(){
    int lcv;
    FILE *ref = fopen("reference_string.txt", "w+");

	fprintf(ref, "%i\n", 0);
    lcv = 1;

    while (lcv < PROC_POOL_SIZE){
        int randNum, reference, q;

        // To simulate locality, a page has a 1/5 chance of
		// being accessed again
        randNum = rand() % 5;
		reference = (double) rand() / (RAND_MAX+1.0) * (MAX_NUM_PAGES);

		for (q = 0; q < randNum; ++q){
			fprintf(ref, "%i\n", reference);
			++lcv;
		}
    }

	// Close the file pointer
	fclose(ref);
}
