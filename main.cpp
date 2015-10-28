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

using namespace std;
using namespace boost;

//V1
char * NULLVAL;
string input;
char * cstr;

bool promptUser(){
	cout << "$ ";
	getline(cin, input);
	while(input != "quit"){
//v1		
		cstr = new char[input.length()+1];	 
		strcpy(cstr, input.c_str());
		cstr = strtok(cstr, " ");
		
	//	int counter = 0;
	//	char * test = cstr;
//		while((test = strchr(cstr, ' ')) != NULL){
//			count++;
//			test++;
//		}
			
//		const int count=counter+1;
//		char ** fullInput = new char*[count];

//		for(int i = 0; i < counter; i++)
//		{
//			fullInput[i] = new char[strlen(cstr[i]) + 1];
//			strcpy(fullInput[i], cstr[i]);
//		}
/*}
		pid_t c_pid, pid;
		int status;
		c_pid = fork();
		if( c_pid < 0 ){
			perror("fork failed");
			exit(1);
		}
		else if (c_pid == 0){
			execvp(cstr[0], cstr);
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
*/

		input="";
		delete[] cstr;
		cout << "$ ";
		getline(cin, input);
	}
	return false;
}

int main(int argc, char * argv[]){

while(promptUser());

return 0;

}
