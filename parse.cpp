#include "parse.h"

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

#include "buildinFunc.h"
#include "shell.h"

using namespace std;
// been tested
vector<string> parse_path(string env) {
  vector<string> parsed_path;
  size_t front = 0;
  size_t rear = 0;
  rear = env.find(":");
  while (rear != string::npos) {
    parsed_path.push_back(env.substr(front, rear - front));
    front = rear + 1;
    rear = env.find(":", front);
  }
  parsed_path.push_back(env.substr(front, env.length() - front));
  return parsed_path;
}

// been tested
string search_path(string command_input, string env) {
  DIR * dirptr;
  struct dirent * entry;
  string full_path("no_such_path");
  // if command_input contains "/", then check whether the command exist in that path
  if (command_input.find("/") != string::npos) {
    ifstream read(command_input.c_str());
    if (read.is_open()) {
      full_path = command_input;
    }
  }
  else {
    // if command_input doesn't include path, then search each directory using loop
    vector<string> parsed_path = parse_path(env);

    for (vector<string>::iterator it = parsed_path.begin(); it != parsed_path.end();
         ++it) {
      dirptr = opendir((*it).c_str());
      // check whether the path exist(it must exist)
      if (dirptr == NULL) {
        perror("no such directory");
        return "no_such_path";
      }

      else {
        // use loop to search in the directory
        while ((entry = readdir(dirptr))) {
          string string_d_name(entry->d_name);
          if (command_input == string_d_name) {
            full_path = (*it) + "/" + command_input;
            break;
          }
        }
      }
      closedir(dirptr);
    }
  }
  return full_path;
}
// been tested
void parse_command(vector<string> & command, string & command_input) {
  getline(cin, command_input);
  if (cin.eof()) {
    exit(EXIT_SUCCESS);
  }
  string buf("");
  bool in_double_quotes = false;
  // Process the string
  string::iterator it = command_input.begin();
  while (it != command_input.end()) {
    if (*it == ' ') {
      if (in_double_quotes) {
        buf.push_back(*it);
        it++;
        continue;
      }
      else {
        if (buf != "") {
          command.push_back(buf);
        }
        buf.clear();
        it++;
        continue;
      }
    }
    else if (*it == '"') {
      in_double_quotes = !in_double_quotes;
      it++;
      continue;
    }
    else if (*it == '\\') {
      buf.push_back(*(it + 1));
      it += 2;
      continue;
    }

    else {
      buf.push_back(*it);
      it++;
      continue;
    }
  }
  if (buf != "") {
    command.push_back(buf);
  }

  if (in_double_quotes) {
    perror("Invalid arguments");
    exit(EXIT_FAILURE);
  }
  return;
}

// test parse_command
void test_parse_command() {
  string test = "./myProgram  \"\\\"hello world\\\"\"   \"a\\\\b\\\\c d\\\\\"";
  vector<string> parsed;
  parse_command(parsed, test);
  for (vector<string>::iterator it = parsed.begin(); it != parsed.end(); ++it) {
    cout << *it << endl;
  }
}
// for set command, been tested
void parse_buildinFunc(vector<string> & buildin_command, string & command_input) {
  // for debug
  /*
  for (size_t i = 0; i < buildin_command.size(); i++) {
    cout << buildin_command[i] << endl;
  }
  cout << command_input << endl;
  */
  buildin_command.clear();
  size_t front = 0;
  size_t rear = command_input.find(" ");
  if (rear != string::npos) {
    // get command_input[0], which is "set"
    if (command_input.substr(front, rear) != "") {
      buildin_command.push_back(command_input.substr(front, rear));
    }
    front = rear + 1;
    rear = command_input.find(" ", front);
    // get command_input[1], which is the var
    if (rear != string::npos && rear != front) {
      if (command_input.substr(front, rear) != "") {
        buildin_command.push_back(command_input.substr(front, rear - front));
      }
      front = rear + 1;
      rear = command_input.find(" ", front);
      // get command_input[2], which is the value, and should be the last argument
      if (rear == string::npos) {
        buildin_command.push_back(
            command_input.substr(front, command_input.length() - front));
        return;
      }
      else {
        cout << "Invalid set argument: more than 2" << endl;
        //exit(EXIT_FAILURE);
        return;
      }
    }
    else {
      cout << "Invalid set argument2: only one" << endl;
      //exit(EXIT_FAILURE);
      return;
    }
  }
  else {
    cout << "Invalid set argument: zero" << endl;
    //exit(EXIT_FAILURE);
    return;
  }
}

//been tested
vector<string> parse_dollar_and_replace(vector<string> & parsed_command,
                                        map<string, string> & var_map) {
  vector<string> parsed_dollar;
  for (vector<string>::iterator it = parsed_command.begin(); it != parsed_command.end();
       ++it) {
    string string_buf("");
    size_t front = 0;
    size_t rear = (*it).find("$");
    if (rear == string::npos) {
      parsed_dollar.push_back(*it);
      continue;
    }
    if (rear != front) {
      string_buf.append((*it).substr(front, rear - front));
    }
    while (rear != string::npos) {
      front = rear + 1;
      rear = (*it).find("$", front);
      string_buf.append(parse_var((*it).substr(front, rear - front), var_map));
      // for debug
      /*
      cout << "the string_buf in while is : " << string_buf << endl;
      */
    }
    //string_buf.append(parse_var((*it).substr(front, (*it).length() - front), var_map));
    // for debug
    // cout << "the string_buf outside while is : " << string_buf << endl;
    parsed_dollar.push_back(string_buf);
  }
  // for debug
  /*
  for (size_t i = 0; i < parsed_dollar.size(); i++) {
    cout << "in the function parsed dollar is: " << parsed_dollar[i] << endl;
  }
  */
  return parsed_dollar;
}

// been tested
string parse_var(string parsed_command_i, map<string, string> & var_map) {
  string parsed_var("");
  size_t first_invalid = 0;
  size_t len = parsed_command_i.length();
  // parse the first invalid digit
  while (first_invalid < len) {
    if (!valid_char(parsed_command_i[first_invalid])) {
      break;
    }
    first_invalid++;
  }
  map<string, string>::iterator it;
  size_t longest = 0;
  // find the longest variable
  for (size_t i = 0; i <= first_invalid; i++) {
    it = var_map.find(parsed_command_i.substr(0, first_invalid - i));
    if (it != var_map.end()) {
      parsed_var.append(it->second);
      longest = first_invalid - i - 1;

      break;
    }
  }
  // for debug
  // cout << "in parse_var: " << parsed_var << endl;
  parsed_var.append(parsed_command_i.substr(longest + 1, len - longest - 1));
  // for debug
  // cout << "in parse_var: " << parsed_var << endl;
  return parsed_var;
}
