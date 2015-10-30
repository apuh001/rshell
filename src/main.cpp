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
				
				//CHECK WHICH ARE EXECUTED...			
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
					execvp(cmds[0], cmds);
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
	
