#include "shell.h"

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
#include "parse.h"
using namespace std;

shell::shell() {
  env = string(getenv("ECE551PATH"));
  parse_command(parsed_command, command_input);
  exit_status = false;
  buidin_status = true;
}
map<string, string> shell::var_map;
void shell::check_command() {
  // for debug
  // cout << var_map.size() << endl;
  if (parsed_command.size() == 0) {
    return;
  }
  // for debug
  /*
  for (size_t i = 0; i < parsed_command.size(); i++) {
    cout << "unreplaced command is: " << parsed_command[i] << endl;
  }
  */
  parsed_command = parse_dollar_and_replace(parsed_command, var_map);

  // for debug
  /*
  for (size_t i = 0; i < parsed_command.size(); i++) {
    cout << "parsed dollar is: " << parsed_command[i] << endl;
  }
  */
  if (parsed_command[0] == "exit") {
    exit_status = true;
    return;
  }
  if (parsed_command[0] == "cd") {
    change_directory(parsed_command);
    return;
  }

  if (parsed_command[0] == "export") {
    export_env(parsed_command, var_map);
    cout << "the exported env is: " << getenv(parsed_command[1].c_str()) << endl;
    return;
  }

  if (parsed_command[0] == "rev") {
    reverse_var(parsed_command, var_map);
    return;
  }

  if (parsed_command[0] == "set") {
    string command_input_for_set;
    if (parsed_command.size() == 3) {
      for (size_t i = 0; i < parsed_command.size() - 1; i++) {
        command_input_for_set.append(parsed_command[i]);
        command_input_for_set.append(" ");
      }
      command_input_for_set.append(parsed_command[parsed_command.size() - 1]);
    }

    else {
      command_input_for_set = command_input;
    }
    // for debug
    // cout << command_input_for_set << endl;

    set_variable(parsed_command, command_input_for_set, var_map);
    // for debug
    /*
    cout << var_map.size() << endl;
    for (size_t i = 0; i < parsed_command.size(); i++) {
      cout << "in if, after set: " << parsed_command[i] << endl;
    }
    */
    parsed_command = parse_dollar_and_replace(parsed_command, var_map);
    /*
    for (size_t i = 0; i < parsed_command.size(); i++) {
      cout << "in if, after parse_dollar: " << parsed_command[i] << endl;
    }
    */
    return;
  }
  string command_buf = parsed_command[0];
  // search path of parsed_command[0]
  parsed_command[0] = search_path(parsed_command[0], env);

  if (parsed_command[0] == "no_such_path") {
    cout << "Command " << command_buf << " not found" << endl;
    return;
  }

  buidin_status = false;
}

void shell::run_command() {
  // convert string to modifiable char *
  char ** newargv = new char *[parsed_command.size() + 1];
  for (size_t i = 0; i < parsed_command.size(); i++) {
    newargv[i] = new char[parsed_command[i].size()];
    strcpy(newargv[i], parsed_command[i].c_str());
  }
  newargv[parsed_command.size()] = NULL;

  execve(parsed_command[0].c_str(), newargv, environ);
  for (size_t i = 0; i < parsed_command.size(); i++) {
    delete[] newargv[i];
  }
  delete[] newargv;

  perror("execve");
  exit(EXIT_FAILURE);
}
