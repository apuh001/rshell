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
	
		if(input.find_first_not_of(' ')!=string::npos && input!="quit"){	
			vector<string> words;
			split(words, input, is_any_of(" "), token_compress_on);		
		
			//THIS WORKED
			
	        	for(auto it=words.begin(); it != words.end(); it++){
				cout << *it << endl;
			}
			

			//CODE TO "CONVERT" VECTOR TO POINTER TO A CSTRING ARR
			cmds = new char * [words.size() + 1];
			
			for(int i = 0; i < words.size(); i++)
			{
				cmds[i] = new char[(words.at(i)).size() + 1];
			        copy(words.at(i).begin(), words.at(i).end(), cmds[i]);
				cmds[i][words.at(i).size()] = '\0';	
			}

				
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
			else if (c_pid > 0)
			{
				if((pid=wait(&status)) < 0)
				{
					perror("wait");
					exit(1);
				}
			}

			for(int i = 0; i < words.size(); i++){
				delete[] cmds[i];
			}	
			delete[] cmds; 		
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
	
