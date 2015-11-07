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

char SPACE = ' ';
string input;
char ** cmds; 

void print_vec(vector<string>& v){
    for(unsigned i = 0; i < v.size(); i++)
	cout << v[i] << endl;
}

//Tokenizes a string into passed in vector
void mystrtok(string& input, vector<string>&all){
//cout << "I'm currently toking..." << endl;
    //CODE TO LOOK FOR CONNECTORS like ; || && and also comments like #
    //FIRST REMOVE ALL COMMENTS BUT CHECK TO SEE THAT ONE MAY POTENTIALLY EXIST FIRST
    if(input.find('#') != string::npos){
        int sharp = input.find('#');
	    //cuts off the part with sharp to the end;
	    input = input.substr(0, sharp-1);
    }	
//cout << "Dont with comments." << endl;	
    //CHECKS FOR CONNECTORS AND ADD SPACES IF NECESSARY
    //lko for last known occurence
    unsigned lko = 0;
    while(input.find("||", lko)!= string::npos){
	lko = input.find("||", lko);
        if((lko != 0) && (input.at(lko-1) != SPACE)){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=3;
	    if(lko < input.size() && input.at(lko) != SPACE){
	    	input = input.substr(0, lko) + SPACE + input.substr(lko);
	    	lko++;
		}	
	}
	else if(lko < input.size() && input.at(lko+2) != SPACE){
	    input = input.substr(0, lko+2) + SPACE + input.substr(lko+2);
	    lko+=3;
	}
	else
	    lko+=2; 	
//cout << input << endl;
//string yourmother;
//cin >> yourmother;
    }
    lko = 0;
    while(input.find("&&", lko)!= string::npos){
	lko = input.find("&&", lko);
        if((lko != 0) && (input.at(lko-1) != SPACE)){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=3;
	    if(lko < input.size() && input.at(lko) != SPACE){
	    	input = input.substr(0, lko) + SPACE + input.substr(lko);
	    	lko++;
		}	
	}
	else if(lko < input.size() && input.at(lko+2) != SPACE){
	    input = input.substr(0, lko+2) + SPACE + input.substr(lko+2);
	    lko+=3;
	}
	else
	    lko+=2; 	
    }	
		
//cout << "Done with ||" << endl;

    lko = 0;
    while( input.find(";", lko)!= string::npos){
    	lko = input.find(";", lko);
	//Add space if theres not already a space before
        if((lko != 0) && (input.at(lko-1) != SPACE)){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=2;
	    if(lko < input.size() && lko + 1 < input.size() && input.at(lko) != SPACE){
	    	input = input.substr(0, lko) + SPACE + input.substr(lko);
	    	lko++;
		}	
	}
	else if(lko < input.size() && lko+1 < input.size() && input.at(lko+1) != SPACE){
	    input = input.substr(0, lko+1) + SPACE + input.substr(lko+1);
	    lko+=2;
	}
	else
	    lko+=1; 	
    }
    
    //Now we can tokenize properly		
    split(all, input, is_any_of(" "), token_compress_on);
}

bool promptUser(){

//cout << "prompted user" << endl;
    cout << "$ ";
    getline(cin, input);

//cout << "Done getting input" << endl;
    while(input != "quit"){
	//Holds all commands;
	vector<string>allCmds;
	if(input.find_first_not_of(' ')!=string::npos && input!="quit" && input.at(0)!='#'){	
	    mystrtok(input, allCmds);		
//////////////TEST
		print_vec(allCmds);
/////////////

//cout << "Done toking" << endl;

	    //PHASE 2: TRACK WHICH ARE EXECUTED
	    //AVAILABLE: begin, ;, &&, || or unknown
	    //c_c for current connector.
	    string c_c = "begin";
            //Beginning of next command sequence.	
	    //unsigned begin_next = 0;

	    //Assume success unless failed by Execvp
	    bool succeeded = 1;
	    bool init = 1; 
	    //Checking to see if this is the first cmd or if this is a cmd following && 
	    //and a previously successful cmd OR if cmd followed by || after a failed cmd
	    //previously OR if cmd is followed by ; which allows immediate execution.
 
	    //c_p for connector place, which runs until a connector is hit
	    unsigned c_p = 0, c_s = 0;
	    //c_s for command start, where the current  command seq starts
	    
	    unsigned sz;

	    while(c_p < allCmds.size()){
	    	//Calc the index of the next connector first to get end of command seq
	    	for(; c_p < allCmds.size()
	            && (allCmds[c_p] != ";" && allCmds[c_p] != "&&" && allCmds[c_p] != "||"); c_p++);
//cout << "What is c_p" << c_p << endl;	
//cout << "allCmds" << allCmds.size() << endl;
//cout << "Whats at c_c" << c_c << endl;
	        unsigned next_inx = 0;	
	        if(init || c_c == ";" || c_c == "end" || (succeeded && c_c == "&&") || (!succeeded && c_c == "||")){    
		    //Set new connector
		    if(c_p < allCmds.size())
		        c_c = allCmds[c_p];
		    else
		        c_c = "end";

		    //connector found
	            init = 0;
		    //if(c_p != allCmds.size()){
		    sz = c_p - c_s;
		   
	   	    cmds = new char * [sz + 1];
		    //From first part of cmd to last part of cmd
		    for(; c_s < c_p; c_s++){ 
			//Create a new c_string
		        cmds[next_inx] = new char[allCmds[c_s].size() + 1];
		        copy(allCmds.at(c_s).begin(), allCmds.at(c_s).end(), cmds[next_inx]);
		        //strcpy(cmds[c_s], allCmds[c_s].c_str());
			cmds[next_inx][allCmds[c_s].size()] = '\0';
			//strcat(cmds[c_s], "\0");
			next_inx++; 
		    }
//cout << "Got here1"<< endl;
		    cmds[next_inx] = NULL; //APPEND NULL
		    //New c_c value is the c_p we computed earlier in the 2nd for loop above.
		    if(c_p < allCmds.size()){
		        //set c_p = sz if it is not after end (sz is one after the connector)
		        //set new c_s as sz, again one after connector
		        c_p++;
		        c_s = c_p;
		    }		
//cout << "Got here" << endl;

	        //PHASE 3: EXECUTION AND TRACK WHICH SUCCEEDED	


////////////////TEST
		//CODE TO "CONVERT" VECTOR TO POINTER TO A CSTRING ARR
/*				cmds = new char * [allCmds.size() + 1];
			
				for(unsigned i = 0; i < allCmds.size(); i++){
					cmds[i] = new char[(allCmds.at(i)).size() + 1];
			        	copy(allCmds.at(i).begin(), allCmds.at(i).end(), cmds[i]);
					cmds[i][allCmds.at(i).size()] = '\0';	
				}
		        	//REMEMBER TO APPEND NULL AT END OF VECTOR
		        	cmds[allCmds.size()] = NULL;
//////////////////////////////////////
*/
		//EXECUTE	
		    pid_t c_pid;
	   	    int status = 0;
		    c_pid = fork();
	
	            if( c_pid < 0 ){
		        perror("fork failed");
		        exit(1);
		    }
		    else if (c_pid == 0){
		        int exec = execvp(cmds[0], cmds);
		        if(exec < 0){
			    perror("Execvp failed");
			    exit(1);
			    succeeded = 0;
		        }
			else 
		            succeeded = 1;
	            }
		    else if (c_pid > 0){
//cout << "Stuck in limbo" << endl;
		        waitpid(-1, &status, 0);
		    }
		
		    //AFTER EXECUTION DELETE EVERYTHING
		    for(unsigned i = 0; i < sz; i++){
		        cmds[i] = NULL;
		    }	
		    //delete[] cmds;	
	    	}
		else{
		    c_p++;
		    c_s = c_p;	
		}
	    
        }


	}
	fflush(stdout);
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
/*    lko = 0;
    while(input.find("&&", lko)!= string::npos){
        lko = input.find("&&", lko);
	//Add space if theres not already a space before
        if(lko != 0 && input.at(lko-1) != SPACE){
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
	    lko+=3;
	}
	if(lko != input.size() && input.at(lko) != SPACE)
	    input = input.substr(0, lko) + SPACE + input.substr(lko);
    }
*/	
