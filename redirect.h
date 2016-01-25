#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <string>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

using namespace std;
//Search Path function to search command                                                                                                                        
//in each path environmental variable                                                                                                                           
string SearchPath(vector<string>& args) {
  int size = 0; // use for storing the size o PAHTS                                                                                                             
  char* PATH = getenv("PATH");
  vector<string> PATHS = parseenv(PATH, &size);
  for(int i = 0; i < size; i++) {
    DIR* dir;
    struct dirent* ent;
    if(PATHS[i].empty()) {
      cout<<"empty PATHS occurs!"<<endl;
      continue;
    }
    if((dir = opendir(PATHS[i].c_str())) != 0) {
      while((ent = readdir(dir))) {
        if(ent->d_name == args[0]) {
          args[0] = PATHS[i] + "/" + args[0];//made up the whole path                                                                                           
          closedir(dir);
          return args[0];
        }
      }
      closedir(dir);
    }
    else {
      cout<<"CANNOT OPEN THE DIRECTORY!"<<endl;
      closedir(dir);
      exit(EXIT_FAILURE);
    }
  }
  cout<<"Command "<<args[0]<<" not found"<<endl;
  return args[0];
}

//redirect function used for reconnect input                                                                                                                    
//output and implement redirect function  
void redirect (string file, int flag) {
    if(flag == 0) {
      int filedes0 = open(file.c_str(), O_RDONLY); //open for read only                                                                                         
      dup2(filedes0, 0); //duplicate another descriptor                                                                                                         
      close(filedes0);
    }
    //redirect output                                                                                                                                           
    else if(flag == 1) {
      int filedes1 = open(file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);//open for read&write,file do not exist create                                        
      dup2(filedes1, 1);
      close(filedes1);
    }
    //redirect error                                                                                                                                            
    else if(flag == 2) {
      int filedes2 = open(file.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
      dup2(filedes2, 2);
      close (filedes2);
    }
  return;
}

//pipeline function used for pipe()                                                                                                                             
//implement intercommunication between programs                                                                                                                 
void pipeline(vector<string>&front, vector<string>&args, size_t* k) {
  int pipefd[2];
  if(pipe(pipefd) == -1) {
    cout<<"Error pipe"<<endl;
    return;
  }
  int status;
  pid_t pid1 = fork(); //2nd fork for program before "|"                                                                                                        
  int sz1 = front.size()+1;
  //change vector<string> into char**                                                                                                                           
  //for execve to execute                                                                                                                                       
  char** cmd = new char*[sz1];
  for(size_t i = 0; i < front.size(); i++) {
    char* argv = (char*)front[i].c_str();
    cmd[i] = argv;
  }
  cmd[sz1-1] = NULL;
  if(pid1 == -1) {
    perror("2nd fork error!");
  }
  if(pid1 == 0) {    //child process                                                                                                                            
    //close current pipe
     close(pipefd[0]);
    dup2(pipefd[1],1);
    close(pipefd[1]);
    execve(cmd[0], cmd, environ);
    perror("2nd execve error!");
    delete[] cmd;
  }
  else {
    close(pipefd[1]);
    dup2(pipefd[0],0);
    close(pipefd[0]);
    delete[] cmd;
    args.erase(args.begin(), args.begin()+(*k)+1);//erase after we deal with this command                                                                       
    if(args[0].find('/') == std::string::npos) {
      args[0] = SearchPath(args);
    }
    if(waitpid(pid1, &status,0) < 0) {
      perror("watipid error");
    }
    else {
      (*k) = 0;
    }
     }
  return;
}


