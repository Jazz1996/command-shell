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
bool valid_variable(string value);
bool valid_char(char input);
void set_variable(vector<string> & parsed_command,
                  string & command_input,
                  map<string, string> & var_map);
void test_set();
void export_env(vector<string> & parsed_command, map<string, string> & var_map);
void reverse_var(vector<string> & parsed_command, map<string, string> & var_map);
void change_directory(vector<string> & parsed_command);
