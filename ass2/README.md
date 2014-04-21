COMP9319
========
Web data compression and search

Assignment 2:
COMP9319 2014s2 Assignment 2: Searching BWT Encoded File

Your task in this assignment is to create a simple search program that implements BWT backward search, which can efficiently search a BWT encoded file. The program also has the capability to decode the BWT encoded file back to its original file in a lossless manner. The original file (before BWT) is delimited by new lines. Sample input files (BWT encoded) have been provided in the folder ~cs9319/a2. To help in testing your program, the corresponding original files (after reverse BWT) are also included in that folder. Although you do not need to submit a BWT encoder, it is highly recommended and assumed that you will implement a simple BWT encoding program (this will help you in understanding the lecture materials and assist in testing your assignment). Your C/C++ program, called bwtsearch, accepts the path to a BWT encoded file; the path to an index file; and one quoted query string (optional) as commandline input arguments. If no query string is given, a reverse BWT is performed and the original file (before BWT) should be output. Using the given query string, it will perform a backward search on the given BWT encoded file and output all the lines that contain the input query string to the standard output. The search results do not need to be sorted according to their line numbers. But if they are sorted, bonus marks will be awarded (refer to the Bonus section below for details). Note that you do not need to print the line numbers but just the line content itself is sufficient. 

The first four bytes (an int) of each given BWT encoded file are reserved for storing the position (zero-based) of the BWT array that contains the last character. As a result, a given BWT encoded file in this assignment is 4 bytes larger than its original text file. For example, if the original text file contains only banana$, then the BWT encoded file will be 11 bytes long. The first four bytes contain the integer 4 and the rest of the bytes contain annb$aa. i.e., The last character is at position 4 (= the fifth character since it is zero-based). 

Since each line is delimited by a newline character, your output will naturally be displayed as one line (ending with a '\n') for each match. No line will be output more than once, i.e., if there are multiple matches in one line, that line will only be output once. 

Your solution can write out one external index file (at any time) that is no larger than half of the size of the given input BWT file, plus an extra 2048 bytes. If your index file is larger than half of the size of the input BWT file plus 2048 bytes, you will receive zero points for the tests that use that file. You may assume that the index file will not be deleted during all the tests for a given BWT file, and all the test BWT files are uniquely named. Therefore, to save time, you only need to generate the index file when it does not exist yet.
Example

Consider one of the given sample files tiny.bwt. You can find the file by logging into CSE machines and going to folder ~cs9319/a2. The original file (tiny.txt) before BWT is also provided in that folder. Given the command:
%wagner> bwtsearch ~cs9319/a2/tiny.bwt ./tiny.idx "each"
The output should be:
6. peach
11. peach
i.e.,
%wagner> bwtsearch ~cs9319/a2/tiny.bwt ./tiny.idx "each"
6. peach
11. peach
%wagner> 
Another example:
%wagner> bwtsearch ~cs9319/a2/gcc.bwt ~/testing/gcc.idx "when not us"
           useful when not using the GNU assembler.
%wagner> 
When no query string is given, the original file before BWT should be output. To avoid displaying too much on the console, you should always pipe the output to a file. For example,
%wagner> bwtsearch ~cs9319/a2/gcc.bwt ~/testing/gcc.idx > gcc.unbwt
%wagner> diff ~cs9319/a2/gcc.txt gcc.unbwt
%wagner> 
As discussed in the lecture on April 17, alternatively, you may output the recovered original text directly to a file using the -o FILENAME option. For example,
%wagner> bwtsearch ~cs9319/a2/gcc.bwt ~/testing/gcc.idx -o gcc.unbwt
%wagner> diff ~cs9319/a2/gcc.txt gcc.unbwt
%wagner>
You do not need to implement both options above (i.e., one of them is sufficient). The auto-testing script will first attempt the -o option. If it does not work, it will try the pipe option. 

We will use the make command below to compile your solution. Please provide a makefile and ensure that the code you submit can be compiled. Solutions that have compilation errors will receive zero points for the entire assignment.
    make bwtsearch
Your solution will be compiled and run on a typical CSE Linux machine e.g. wagner. Your solution should not write out any external files other than the index file. Any solution that writes out external files other than the index file will receive zero points for the entire assignment.
Performance

Your solution will be marked based on space and runtime performance. Your soluton will not be tested against any BWT encoded files that are larger than 100MB. 

Runtime memory is assumed to be always less than 2MB. Runtime memory consumption will be measured by valgrind massif with the option --pages-as-heap=yes, i.e., all the memory used by your program will be measured. Any solution that violates this memory requirement will receive zero points for that query test. 

Any solution that runs for more than 120 seconds on a machine with similar specification as wagner for the first query on a given BWT file will be killed, and will receive zero points for the queries for that BWT file. After that, any solution that runs for more than 30 seconds for any one of the subsequent queries on that BWT file will be killed, and will receive zero points for that query test. We will use the time command and count both the user and system time as runtime measurement. 

When testing for reverse BWT (i.e., decoding the BWT encoded file back to the original text file), you may assume that at least one query will be run before hand for a given test file, and you will have up to 120 seconds before your running process is killed.
Documentation

You will be marked on your documentation of your comments for variables, functions and steps in your solution. Your source code will be inspected and marked based on readability and ease of understanding.
Assumptions/clarifications/hints

You can assume that all test files are originally in UTF8, and they are then BWT transformed in a byte-based manner. i.e., you need to assume that each character may use up the full 8 bits.
To avoid a long output time, none of the testcases for marking will result in outputting more than 100 lines.
A line will not be unreasonably long, e.g., you will not see a line that is 10,000+ chars long.
The input filename is a path to the given BWT encoded file. Please open the file as read-only, in case you do not have write permissions.
Marks will be deducted for the output of any extra text, other than the required correct answers (in the right order). This extra information includes (but not limited to) debugging messages, line numbers and so on.
You can assume that the input query string will not be an empty string (i.e., "").
You can assume that every line (including the last line) of the original file corresponding to the input BWT file ends with a newline char.
Empty lines may exist in the original files (before BWT). However, these lines will never be matched during searching because the empty string will not be used when testing your program.
Marking

This assignment is worth 100 points. Below is an indicative marking scheme:
Component	Points
Auto marking (small file up to 5MB)	50
Auto marking (large file up to 100MB)	45
Documentation	5
Bonus

Bonus marks (up to 10 points) will be awarded for the solution that achieves 100 points, uses overall minimum amount of runtime memory and runs the fastest overall for large files (i.e., the shortest total time to finish all the tests for the largest file in the auto marking). This solution will be shared with the class. In addition, bonus marks of 5 points will be awarded for the solution that achieves 100 points and outputs all the search results sorted according to their line numbers. Note: regardless of the bonus marks you receive in this assignment, the maximum final mark for the subject is capped at 100.
Submission

Deadline: Friday 2nd May 23:59. Late submission will attract 10% penalty for the first day and 30% penalty for each subsequent day. Use the give command below to submit the assignment:
    give cs9319 a2 makefile *.c *.cpp *.h
Please use classrun to check your submission to make sure that you have submitted all the necessary files.
Plagiarism

The work you submit must be your own work. Submission of work partially or completely derived from any other person or jointly written with any other person is not permitted. The penalties for such an offence may include negative marks, automatic failure of the course and possibly other academic discipline. Assignment submissions will be examined both automatically and manually for such submissions. 

Relevant scholarship authorities will be informed if students holding scholarships are involved in an incident of plagiarism or other misconduct. 

Do not provide or show your assignment work to any other person - apart from the teaching staff of this subject. If you knowingly provide or show your assignment work to another person for any reason, and work derived from it is submitted you may be penalized, even if the work was submitted without your knowledge or consent. This may apply even if your work is submitted by a third party unknown to you. 


