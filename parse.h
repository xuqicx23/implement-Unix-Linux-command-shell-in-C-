#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
/*****************************************************************/
/*  Functions used for parse environment variable & command line */
/*****************************************************************/
//parse the environment variable                                                                                                                                
vector<string> parseenv(char* PATH, int* size) {
  vector<string>buf;
  string leaf;
  char* ptr = PATH;
  while((*ptr) != '\0') {
    if((*ptr) == ':') {
      buf.push_back(leaf);
      leaf.clear();
      ptr++;
      if((*ptr) == '\0') {
        buf.push_back(leaf);
        leaf.clear();
        (*size)++;
      }
      (*size)++;
    }
    else {
      leaf.push_back((*ptr));
      ptr++;
      if((*ptr) == '\0') {
        buf.push_back(leaf);
        leaf.clear();
        (*size)++;
      }
    }
  }
  return buf;
}

//parse the input command line                                                                                                                                  
//read from the user based on whitespace                                                                                                                        
vector<string> parseline(string line) {
  vector<string>buf; //buf to store the command                                                                                                                 
  const char* ptr = line.c_str();
  string leaf;
  while((*ptr) != '\0') {
    //when we run into whitespace                                                                                                                               
    if((*ptr) == ' ') {
      buf.push_back(leaf);
      leaf.clear();
      ptr++;
      if((*ptr) == '\0') {
        buf.push_back(leaf);
        leaf.clear();
      }
    }
    //when we have"\\" character                                                                                                                                
    else if((*ptr) == '\\') {
      ptr++;
      //"\\whitespace"condition                                                                                                                                 
      if((*ptr) == '\0') {
        buf.push_back(leaf);
        leaf.clear();
      }
      if((*ptr) == ' ') {
        leaf = leaf + " ";
	  ptr++;
        if((*ptr) == '\0') {
          buf.push_back(leaf);
          leaf.clear();
        }
      }
      else {
        //"\\" followed by another character, add both them to the string                                                                                       
        leaf = leaf + "\\";
        if((*ptr) == '\\') {
          continue;
        }
        else {
          leaf.push_back(*ptr);// need to add both the "\\" and "*ptr" to the string                                                                            
          ptr++;
          if((*ptr) == '\0') {
            buf.push_back(leaf);
            leaf.clear();
          }
        }
      }
    }
   //regular condition with no " " or "\\"                                                                                                                     
    else {
      leaf.push_back(*ptr);
      ptr++;
      if((*ptr) == '\0') {
        buf.push_back(leaf);
        leaf.clear();
      }
    }
  }
  //try to erase the whitespace we put in when "\\ "workds                                                                                                     \
                                                                                                                                                                
  for(size_t i = 0; i < buf.size(); i++) {
    if(buf[i] == "") {
      buf.erase(buf.begin() + i);
      i = 0;
    }
  }
  return buf;
}

