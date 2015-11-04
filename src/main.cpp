/*
 * Alexander Puh
 * I shan't cheat. 
 * 10/27/2015
 * CS100 - Rshell
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

//V1
//char * NULLVAL;
string input;
//char * cstr;
char ** cmds; 

bool promptUser(){
	cout << "$ ";
	getline(cin, input);
	while(input != "quit"){
	
		if(input.find_first_not_of(' ')!=string::npos && input!="quit" && input.at(0)!='#'){	
			vector<string> words;
			split(words, input, is_any_of(" "), token_compress_on);		
		
			//THIS WORKED
			/*	
	        	for(auto it=words.begin(); it != words.end(); it++){
				cout << *it << endl;
			}
			
			*/
			//CODE TO LOOK FOR CONNECTORS like ; || && and also comments like #
			//FIRST REMOVE ALL COMMENTS BUT CHECK TO SEE THAT ONE MAY POTENTIALLY EXIST FIRST
			if(input.find('#') != string::npos){
				unsigned earliestComment = words.size(); //Assuming one after last index
				for(unsigned i = 0; i < words.size(); i++){
					//Ensures if sharp is found, it was at the beginning of the word or alone
					if(words[i].find('#')!=string::npos && words[i].at(0)=='#')
						earliestComment=i;
				}
				//REMOVE ALL TOKENS AFTER AND AT THE FOUND COMMENT
				for(unsigned i = words.size(); i >= earliestComment; i--){
					words.pop_back();
				}	
			}		
		
			//CHECKS FOR CONNECTORS
			//BUT FIRST CONFIRM IF THERE ARE CONNECTORS OR NOT
			if(input.find("||")!=string::npos || input.find("&&")!=string::npos || input.find(";")!=string::npos){
				vector<int> conIndices;	//All indices in which a connector was found	
				for(unsigned i = 0; i <words.size(); i++){
					if(words[i].size() > 1)
						if(words[i].at(0)=="||" || words[i].at(0)=="&&" || words[i].at(0)==";")
							conIndices.push_back(i);
				}	

				//NOW SPLIT UP INTO SEVERAL CMDS
				vector<vector<string>> allCmds;
			
				unsigned lastStartVal = 0;
				for(unsigned i = 0; i < conIndices.size(); i++){
					vector<string> vec;
					for(unsigned y = lastStartVal; y < conIndices[i]; y++){
						vec.push_back(words.at(y));
						lastStartVal=conIndices[i]+1;
					}
					vec.push_back(NULL);
					allCmds.push_back(vec);
					vec.clear();
				}
					
				//TRACK WHICH ARE EXECUTED
				bool succeeded = 1;
				//AVAILABLE: begin, ;, &&, ||
				string current_connector = "begin";		
	
				pid_t c_pid, pid;
				int status;
			
				//CONVERT	
				for(unsigned x = 0; x < allCmds.size(); x++){
					//Checking to see if this is the first cmd or if this is a cmd following && 
					//and a previously successful cmd OR if cmd followed by || after a failed cmd
					//previously OR if cmd is followed by ; which allows immediate execution.
					if(current_connector = ";" || 
					   (succeeded && (current_connector = "begin" || current_connector = "&&")) || 
					   (!succeeded && current_connector = "||")){
						cmds = new char * [allCmds[x].size() + 1];
						for(unsigned i = 0; i < allCmds.at(x).size(); i++){
							cmds[i] = new char[(allCmds[x].size() + 1];
							copy(allCmds[x].at(i).begin(), allCmds[x].at(i).end(), cmds[i]);
							cmds[i][allCmds[x].at(i).size()] = '\0';
						}
						//APPENT NULL
						cmds[words.size()] = NULL;
					
						//Assume success unless set to fail by execvp
						succeeded = 1;

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
							perror("Execvp failed");
						}
						else if (c_pid > 0){
							if((pid=wait(&status)) < 0){
								perror("wait");
								exit(1);
							}
						}

						//Set connector
						current_connector = allCmds[x][allCmds[x].size() - 1];

						for(unsigned i = 0; i < words.size(); i++){
							delete[] cmds[i];
						}	
						delete[] cmds;
					}
					//In this case, the last cmd failed followed by && or
					//last cmd succeeded followed by a || which means this
					//cmd has failed = not executed.
					else{
						succeeded = 0;
						//LAST INDEX CARRIES A CONNECTOR
						current_connector = allCmds[x][allCmds[x].size()-1]; 		
					} 					
				}
			}
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
		}
		//RESET ALL VALUES
		input="quit";
		//delete[] cstr;
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


//OLD VERSION 		
		//int counter = 0;
		//char * test = cstr;
		//while((test = strchr(cstr, ' ')) != NULL){
		//	count++;
		//	test++;
		//}
		/*	
		const int count=counter+1;
		char ** fullInput = new char*[count];

		for(int i = 0; i < counter; i++)
		{
			fullInput[i] = new char[strlen(cstr[i]) + 1];
			strcpy(fullInput[i], cstr[i]);
		}
		*/
			
			//OLD VERSION
			//cstr = new char[input.length()+1];	 
			//strcpy(cstr, input.c_str());
			//cstr = strtok(cstr, " ");
	
