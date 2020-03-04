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
class shell {
 public:
  string command_input;
  string env;
  vector<string> parsed_command;
  bool exit_status;
  bool buidin_status;
  static map<string, string> var_map;

  shell();
  void check_command();
  void run_command();
};
