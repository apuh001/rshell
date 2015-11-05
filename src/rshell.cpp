/*
 * Alexander Puh
 * I shan't cheat. 
 * 10/27/2015
 * CS100 - Rshell - rshell.cpp
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

string SPACE = " ";
string input;
char ** cmds; 

//Tokenizes a string into passed in vector
void mystrtok(string& input, vector<string>&all){
    //CODE TO LOOK FOR CONNECTORS like ; || && and also comments like #
    //FIRST REMOVE ALL COMMENTS BUT CHECK TO SEE THAT ONE MAY POTENTIALLY EXIST FIRST
    if(input.find('#') != string::npos){
        int sharp = input.find('#');
	    //cuts off the part with sharp to the end;
	    input = input.substr(0, sharp);
    }		
    //CHECKS FOR CONNECTORS AND ADD SPACES IF NECESSARY
    //lko for last known occurence
    int lko = 0;
    while(input.find("||", lko)!= string::npos){
        lko = input.find("||", lko);
	//Add space if theres not already a space before
        if(lko != 0 && input.at(lko-1) != SPACE){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=3;
	}
	if(lko != input.size() && input.at(lko) != SPACE)
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
    }		

    lko = 0;
    while(input.find("&&", last_known)!= string::npos){
        lko = input.find("&&", lko);
	//Add space if theres not already a space before
        if(lko != 0 && input.at(lko-1) != SPACE){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=3;
	}
	if(lko != input.size() && input.at(lko) != SPACE)
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
    }
	
    lko = 0;
    while( input.find(";", last_known)!= string::npos){
        lko = input.find(";", lko);
	//Add space if theres not already a space before
        if(lko != 0 && input.at(lko-1) != SPACE){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=2;
	}
	if(lko != input.size() && input.at(lko) != SPACE)
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
    }
    
    //Now we can tokenize properly		
    split(all, input, is_any_of(" "), token_compress_on);
}

bool promptUser(){
    cout << "$ ";
    getline(cin, input);
    while(input != "quit"){
	//Holds all commands;
	vector<string>allCmds;
	if(input.find_first_not_of(' ')!=string::npos && input!="quit" && input.at(0)!='#'){	
	    mystrtok(input, allCmds);		

	    //PHASE 2: TRACK WHICH ARE EXECUTED
	    //AVAILABLE: begin, ;, &&, || or unknown
	    //c_c for current connector.
	    string c_c = "begin";
            //Beginning of next command sequence.	
	    begin_next = 0;

	    //Assume success unless failed by Execvp
	    bool succeeded = 1;
	    pid_t c_pid, pid;
	    int status;
	    //Checking to see if this is the first cmd or if this is a cmd following && 
	    //and a previously successful cmd OR if cmd followed by || after a failed cmd
	    //previously OR if cmd is followed by ; which allows immediate execution.
	    
	    //c_p for command place, which runs until a connector is hit
	    int c_p, c_s = 0;
	    //c_s for command start, where the current  command seq starts
	    
	    while(c_p < allCmds.size()){
	    	//Calc the index of the next connector first to get end of command seq
	    	for(; c_p < allCmds.size()
	            && (allCmds[c_p] != ";" && allCmds[c_p] != "&&" && allCmds[c_p] != ";"); c_p++);
		
	        if(c_c = ";" || c_c = "begin" || (succeeded && c_c = "&&") || (!succeeded && c_c = "||")){
	            //connector found
	            int sz;
		    if(c_p != allCmds.size(){
			sz = c_p + 1;
			cmds = new char * [sz + 1];}
		    else{
			sz = c_p;
			cmds = new char * [sz + 1];}
		    //From first part of cmd to last part of cmd
		    for(; c_s < sz; c_s++){ 
			//Create a new c_string
		        cmds[c_s] = new char[allCmds[c_s].size() + 1];
		        copy(allCmds.at(c_s).begin(), allCmds.at(c_s).end(), cmds[c_s]);
		        cmds[c_s][allCmds[c_s].size()] = '\0';
		    }

		    cmds[sz] = NULL; //APPEND NULL
		    
		  //set c_p = sz if it is not after end (sz is one after the connector)
		  //set new c_s as sz, again one after connector
		  c_p = sz;
		  c_s = sz;
		}		

	        //PHASE 3: EXECUTION AND TRACK WHICH SUCCEEDED	

		//EXECUTE	
		pid_t c_pid, pid;
		int status;
					
	        if( c_pid < 0 ){
		    perror("fork failed");
		    exit(1);
		}
		else if (c_pid == 0){
		    if (execvp(cmds[0], cmds) < 0){
			cout << "EXECVP FAILED\n";
			succeeded = 0;
		    }
		    else
			succeeded = 1;
		    perror("Execvp failed");
	        }
		else if (c_pid > 0){
		    if((pid=wait(&status)) < 0){
		        perror("wait");
			exit(1);
		    }
		}
		
		//AFTER EXECUTION DELETE EVERYTHING
		for(unsigned i = 0; i < sz; i++){
			delete[] cmds[i];
		}	
		delete[] cmds;		
	    }
        }

	//RESET ALL VALUES
	input="quit";
	cout << "$ ";
	cin.clear();
	getline(cin, input);
    }
    return false;
}

int main(int argc, char * argv[]){
	while(promptUser());
	return 0;
}


	
		/*
			//========================================
			//VERSION WHERE THERE ARE NO CONNECTORS...
			//========================================
			else{ 			

				//CODE TO "CONVERT" VECTOR TO POINTER TO A CSTRING ARR
				cmds = new char * [words.size() + 1];
			
				for(unsigned i = 0; i < words.size(); i++){
					cmds[i] = new char[(words.at(i)).size() + 1];
			        	copy(words.at(i).begin(), words.at(i).end(), cmds[i]);
					cmds[i][words.at(i).size()] = '\0';	
				}
		        	//REMEMBER TO APPEND NULL AT END OF VECTOR
		        	cmds[words.size()] = NULL;

				pid_t c_pid, pid;
				int status;
				c_pid = fork();
	        		if( c_pid < 0 ){
					perror("fork failed");
					exit(1);
				}
				else if (c_pid == 0){
					if (execvp(cmds[0], cmds) < 0){
						cout << "EXECVP FAILED\n";
						exit(1);
				}
					perror("Execvp failed");
				}
				else if (c_pid > 0){
					if((pid=wait(&status)) < 0){
						perror("wait");
						exit(1);
					}
				}

				cout << "The status is: " << status << endl;

				for(unsigned i = 0; i < words.size(); i++){
					delete[] cmds[i];
				}	
				delete[] cmds; 		
			}
		*/

