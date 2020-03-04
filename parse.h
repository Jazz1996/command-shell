#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
// Parse the path with ":"
vector<string> parse_path(string env);

// Search the command in the environment path
string search_path(string command_input, string env);

// Parse the command
void parse_command(vector<string> & command, string & command_input);

// Use an instance to test parse_command function
void test_parse_command();

// Parse the set command
void parse_buildinFunc(vector<string> & buildin_command, string & command_input);

// Replace the arguments which use $ variable
vector<string> parse_dollar_and_replace(vector<string> & parsed_command,
                                        map<string, string> & var_map);

// Search for the longest variable in the map
string parse_var(string parsed_command_i, map<string, string> & var_map);
