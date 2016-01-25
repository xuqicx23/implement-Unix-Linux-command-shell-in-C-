#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
using namespace std;
/************************************************/
/*  Lists all the builtin functions for shell   */
/************************************************/

// pushd command:cd to the argument directory & push old to the stack                                                                                           
int pushdfunc(vector<string>args, char** stack, int* index) {
  if(args.size() != 2) {
    cout<<"input one directory for push!"<<endl;
    return 1;
  }
  if(args[1].empty()) {
    cout<<"No diectory for push"<<endl;
    return 1;
  }
  DIR* dir;
  dir = opendir(args[1].c_str());
  if(dir != NULL) {
  stack[(*index)] = getcwd(NULL, 0);
  chdir(args[1].c_str());
  closedir(dir);
  }
  else {
    cerr<<"directory does not exist!"<<endl;
    closedir(dir);
    return 1;
  }
  if((*index) < 1023) {
    (*index)++;
  }
  else {
    cerr<<"stack is full!"<<endl;
  }
  return 1;
}

//popd command cd to the top of the stack and remove that from the stack                                                                                        
int popdfunc(vector<string>args, char** stack, int* index) {
  if((*index) != 0) {
    chdir(stack[(*index) - 1]);
    free(stack[(*index) - 1]);
    (*index)--;
  }
  else {
      cerr<<"empty stack!"<<endl;
      return 1;
  }
  return 1;
}

//Lists the directory stack, top last and bottom first                                                                                                          
int dirstackfunc(char** stack, int* index) {
  if((*index) == 0) {
    cout<<"empty directory stack!"<<endl;
    return 1;
  }
   for(int i = 0; i < (*index); i++) {
    cout<<stack[i]<<endl;
  }
  return 1;
}

// cd to the argument directory                                                                                                                                 
int cdfunc(vector<string>args) {
  if(args.size() != 2) {
    cout<<"input one directory for cd!"<<endl;
    return 1;
  }
  if(args[1].empty()) {
    cout<<"No directory for cd!"<<endl;
    return 1;
  }
  if(chdir(args[1].c_str()) == -1) {
    cout<<args[1]<<" not a directory!"<<endl;
    return 1;
  }
  else {
  return 1;
  }
}

//using for exit myshell                                                                                                                                        
int exitfunc(vector<string>args,char** stack, int* index) {
  if((*index) != 0) {
  for(int i = 0; i < (*index); i++){
    free (stack[i]);
  }
  }
  delete[] stack;
  return 0;
}

