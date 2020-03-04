#include "buildinFunc.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "parse.h"
using namespace std;
// been tested
bool valid_variable(string value) {
  for (string::iterator it = value.begin(); it != value.end(); ++it) {
    if (!((*it >= '0' && *it <= '9') || (*it >= 'A' && *it <= 'Z') ||
          (*it >= 'a' && *it <= 'z') || (*it == '_'))) {
      return false;
    }
  }
  return true;
}
// been tested
bool valid_char(char input) {
  if (!((input >= '0' && input <= '9') || (input >= 'A' && input <= 'Z') ||
        (input >= 'a' && input <= 'z') || (input == '_'))) {
    return false;
  }
  return true;
}
// been test
void set_variable(vector<string> & parsed_command,
                  string & command_input,
                  map<string, string> & var_map) {
  parse_buildinFunc(parsed_command, command_input);
  // for debug
  /*
  cout << "in set_var func: " << parsed_command[1] << endl;
  cout << "in set_var func: " << parsed_command[2] << endl;
  */
  if (parsed_command.size() < 3) {
    cerr << "the number of argument is less than 2" << endl;
    return;
  }

  if (!valid_variable(parsed_command[1])) {
    cerr << "variable input is not valid" << endl;
    //exit(EXIT_FAILURE);
    return;
  }
  // if the variable exists, then erase it
  map<string, string>::iterator it = var_map.find(parsed_command[1]);
  if (it != var_map.end()) {
    var_map.erase(it);
  }
  // add pair to the map
  var_map.insert(pair<string, string>(parsed_command[1], parsed_command[2]));
  // for debug
  // cout << var_map.size() << endl;
}

// test set
void test_set() {
  map<string, string> var_map;
  vector<string> parsed;
  string test = "set a b";
  // parse_buildinFunc(parsed, test);
  set_variable(parsed, test, var_map);
  // map<string,string>::iterator it = var_map.find("b");
  cout << var_map.size() << endl;
}

// been tested
void export_env(vector<string> & parsed_command, map<string, string> & var_map) {
  if (parsed_command.size() != 2) {
    cerr << "invalid number of arguments" << endl;
    return;
    //exit(EXIT_FAILURE);
  }
  if (!valid_variable(parsed_command[1])) {
    cerr << "variable input is not valid" << endl;
    //exit(EXIT_FAILURE);
    return;
  }

  map<string, string>::iterator it = var_map.find(parsed_command[1]);
  if (it != var_map.end()) {
    setenv(parsed_command[1].c_str(), it->second.c_str(), 1);
    cout << "env has been set" << endl;
    return;
  }

  cout << "the env you try to export does not exist" << endl;
}

// been tested
void reverse_var(vector<string> & parsed_command, map<string, string> & var_map) {
  if (parsed_command.size() != 2) {
    cerr << "invalid number of arguments" << endl;
    //exit(EXIT_FAILURE);
    return;
  }
  if (!valid_variable(parsed_command[1])) {
    cerr << "variable input is not valid" << endl;
    //exit(EXIT_FAILURE);
    return;
  }
  // for debug
  // cout << "in rev func, parsed_command[1]: " << parsed_command[1] << endl;
  map<string, string>::iterator it = var_map.find(parsed_command[1]);
  if (it != var_map.end()) {
    reverse((it->second).begin(), (it->second).end());
    cout << "the value has been successfully reversed" << endl;
    return;
  }

  cout << "the value you try to rev does not exist" << endl;
}

// been tested
void change_directory(vector<string> & parsed_command) {
  if (parsed_command.size() > 2) {
    perror("Only one arguement is acceptable!");
  }
  else {
    if (parsed_command.size() == 1) {
      chdir(getenv("HOME"));
      return;
    }
    int cd_status = chdir(parsed_command[1].c_str());
    if (cd_status == -1) {
      perror("Can't change to the directory");
    }
  }
}
