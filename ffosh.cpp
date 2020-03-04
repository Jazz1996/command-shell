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
#include "shell.h"

#define MAXPATH 255
using namespace std;
void print_shell() {
  char directory[MAXPATH];
  getcwd(directory, MAXPATH);
  cout << "ffosh:" << directory << " $ ";
}
int main(int argc, const char * argv[]) {
  setenv("ECE551PATH", getenv("PATH"), 1);
  while (true) {
    print_shell();
    shell ffosh;
    ffosh.check_command();
    if (ffosh.exit_status) {
      break;
    }
    if (ffosh.buidin_status) {
      continue;
    }
    int wstatus;
    pid_t fork_pid;
    pid_t wait_pid;
    fork_pid = fork();
    if (fork_pid < 0) {
      perror("error: fork_pid is less than zero");
      exit(EXIT_FAILURE);
    }
    // Code executed by child
    if (fork_pid == 0) {
      ffosh.run_command();
    }
    // Code executed by parent
    else {
      do {
        wait_pid = waitpid(fork_pid, &wstatus, WUNTRACED | WCONTINUED);
        if (wait_pid == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }
        if (WIFEXITED(wstatus)) {
          if (WEXITSTATUS(wstatus) == 0) {
            cout << "Program was successful" << endl;
          }

          else {
            cout << "Program failed with code " << WEXITSTATUS(wstatus) << endl;
          }
        }
        else if (WIFSIGNALED(wstatus)) {
          cout << "Terminated by signal " << WTERMSIG(wstatus) << endl;
        }
      } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    }
  }
  return EXIT_SUCCESS;
}
