#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

#include "builtin.h"
#include "parse.h"
#include "redirect.h"
using namespace std;
//exception class for throw()                                                                                                                               
class myexception: public exception {
  virtual const char* what() const throw() {
    return "command not executable!";
  }
} myexcept;

//Forward declarations for builtin & all other functions                                                                                                    
void redirect(string file, int flag);
void pipeline(vector<string>&front, vector<string>&args, size_t*k);
int cdfunc(vector<string>args);
int exitfunc(vector<string>args, char** stack, int* index);
int pushdfunc(vector<string>args);
int popdfunc(vector<string>args);
int dirstackfunc(vector<string>args, int* index);
vector<string> parseenv(char* PATH, int* size);
string SearchPath(vector<string>&args);
int launch(vector<string>&args);
int execute(vector<string>&args, char** stack, int* index);
vector<string> parseline(string line);
void loop(void);

//function used for run the child/parent process                                                                                                            
//and check for redirection, pipelien                                                                                                                       
int launch(vector<string>&args) {
  pid_t pid;
  pid_t wpid;
  int status;
  pid = fork();
  if(pid == 0) {
    /********************************************/
    //check for IO redirection & Pipeline                                                                                                                   
    for(size_t k = 0; k < args.size(); k++) {
      if(strcmp(args[k].c_str(), "<") == 0) {
        redirect(args[k+1], 0);
        args.erase(args.begin()+k,args.begin()+k+2);
        k = 0;
        continue;
      }
      else if(strcmp(args[k].c_str(), ">") == 0) {
        redirect(args[k+1], 1);
        args.erase(args.begin()+k,args.begin()+k+2);
        k = 0;
        continue;
      }
      else if(strcmp(args[k].c_str(), "2>") == 0) {
        redirect(args[k+1], 2);
        args.erase(args.begin()+k, args.begin()+k+2);
        k = 0;
        continue;
      }
         else if(strcmp(args[k].c_str(), "|") == 0) {
        vector<string>front;
        for(size_t t = 0; t < k; t++) {
          front.push_back(args[t]);
        }
        pipeline(front, args, &k);
        continue;
      }
    }
    /***************************************/
    //execute the program                                                                                                                                   
    int sz = args.size() + 1;
    char** argss = new char*[sz];
    for(size_t i = 0; i < args.size(); i++) {
      if(!args[i].empty()) {
        char* argv = (char*)args[i].c_str();
        argss[i] = argv;
      }
    }
    argss[sz-1] = NULL;
    execve(argss[0], argss, environ);
    perror("Error!");
    delete[] argss;
      throw myexcept;
    //return(EXIT_FAILURE);                                                                                                                                 
  }
  //parent process                                                                                                                                          
  else if(pid < 0) {
    cerr << "fork error!" << endl;
  }
  else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
      if(wpid == -1) {
        cout << "waitpid error!" << endl;
      }
      if(WIFEXITED(status)) {
        cout<<"Program exited with status " << WEXITSTATUS(status) << endl;
      }
      else if(WIFSIGNALED(status)) {
        cout << "Program was killed by signal " << WTERMSIG(status) << endl;
      }
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}
//function checking the command line                                                                                                                        
//then assign to the proper launch or builtin functions                                                                                                     
//in charge of different kinds of executions                                                                                                                
int execute(vector<string>&args, char** stack, int* index) {
  if(args[0].empty()) {
    return 1;
  }
  if(args[0].c_str() == NULL) {
    return 1;
  }
  /******************************************/
  //below map to the specific builtin functions to execute                                                                                                 \
                                                                                                                                                            
  if(strcmp(args[0].c_str(), "cd") == 0) {
    return cdfunc(args);
  }
  else if(strcmp(args[0].c_str(), "exit") == 0) {
    return exitfunc(args, stack, index);
  }
  else if(strcmp(args[0].c_str(), "pushd") == 0) {
    return pushdfunc(args, stack, index);
  }
  else if(strcmp(args[0].c_str(), "popd") == 0) {
    return popdfunc(args, stack, index);
  }
  else if(strcmp(args[0].c_str(), "dirstack") == 0) {
    return dirstackfunc(stack, index);
  }
  /*********************************************/
  if(args[0].find('/') == std::string::npos) {
    args[0] = SearchPath(args);
  }
  if(args[0].find('/') != std::string::npos) {
    return launch(args);
  }
  return 1;
}

//"infinite" loop of shell                                                                                                                                  
void loop(void) {
  std::string line;
  vector<string> args;
  int status = 1;
  char** stack = new char*[1024]; //create stack                                                                                                            
  int index = 0;
  char* pathname;
  do {
    pathname = getcwd(NULL, 0);
    if (pathname == NULL) {
      cout<<"cwd error!"<<endl;
    }
    cout<<"myShell:"<<pathname<<"$ ";
    free(pathname);
    string cmd;
    //read input from user                                                                                                                                  
    getline(cin, cmd);
    if(cin.eof()) {
      status = 0;
      delete[] stack;
    }
    else {
      if(cmd.empty()) {
      status = 1;
      continue;
      }
      args = parseline(cmd);
        status = execute(args, stack, &index);
    }
  } while(status);
}

int main(int argc, char** argv) {
  loop();
  return EXIT_SUCCESS;
}



