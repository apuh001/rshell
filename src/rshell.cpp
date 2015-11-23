/*
 * Alexander Puh
 * I shan't cheat. 
 * 10/27/2015
 * CS100 - Rshell - rshell.cpp
 */

#include <sys/ioctl.h>
//#include <Timer.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <dirent.h>
#include <errno.h>
#include <algorithm>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

using namespace std;
using namespace boost;

//Tracks precedence
bool has_parens = 0;

char SPACE = ' ';
string input;
char ** cmds; 

void print_vec(vector<string>& v){
    cout << "Printing.... " << endl;
    for(unsigned i = 0; i < v.size(); i++)
	cout << v[i] << " ";
    cout << endl;
}

//Use to check if there is an even (modable by 2) 
//number of quotes, parens, braces, brackets 
bool all_occur_closed(string in, string s){
    int occurence = 0;
    string clos = "";

    if(s == "\""){
	int cnt = 0;
	while(in.find(s, occurence) != string::npos){
	    cnt++;
	    occurence = in.find(s, occurence) + 1;
	}
	if(cnt%2==0)
	    return true;
	return false;
    }
    else if(s == "["){
	clos = "]";
    }
    else if(s == "{"){
 	clos = "}"; 
    }
    else if(s == "("){
	clos = ")";
    }

    int cnt_op = 0, cnt_ed = 0;
    while(in.find(s, occurence) != string::npos){
        cnt_op++;
	occurence = in.find(s, occurence) + 1;
    }
    occurence = 0;
    while(in.find(clos, occurence) != string::npos){
	cnt_ed++;
	occurence = in.find(clos, occurence) + 1;
    }
    if(cnt_op == cnt_ed)
	return 1;
    return 0; 
}

void bracket_replace_test(vector<string> & v){
    for(unsigned i = 0; i < v.size(); i++){
	if(v[i] == "[")
	    v[i] = "test";
	if(v[i] == "]")
	    v[i] = "";	
    } 
}

vector<string> vector_retok(vector<string> v){
    vector<string> vec;
    for(unsigned i = 0; i < v.size(); i++){
        if(v[i] != ""){
	    vec.push_back(v[i]);
	}
    }
    return vec;
}

//Tokenizes a string into passed in vector
void mystrtok(string& input, vector<string>&all){
    //Check for occurences of {, [, (, " that all are closed
    if(!all_occur_closed(input, "{") || !all_occur_closed(input, "\"") ||
       !all_occur_closed(input, "[") || !all_occur_closed(input, "(")){
	cout << "Error: Quotes/Parens/Bracket/Braces not closed." << endl;
	input = "";
	return;
    }	

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
   

    //cleaup parens
    lko = 0;
    while( input.find("(", lko) != string::npos){
        lko = input.find("(", lko);
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
 
    lko = 0;
    while( input.find(")", lko) != string::npos){
        lko = input.find(")", lko);
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

    //CHECK FOR [ ] AND CONVERT TO TEST
    //CHECK FOR " ". IF EVEN NUMBER OF " WE GO ON, IF ODD RETURN ERROR
     
    //Now we can tokenize properly		
    split(all, input, is_any_of(" "), token_compress_on);
    bracket_replace_test(all);
    all = vector_retok(all);

    //RETOKENIZE      
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
	//		print_vec(allCmds);
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
	                && (allCmds[c_p] != ";" && allCmds[c_p] != "&&" && allCmds[c_p] != "||"
		        && allCmds[c_p] != "(" && allCmds[c_p] != ")"); c_p++);
	
		//cout << "What is c_p" << c_p << endl;	
		//cout << "allCmds" << allCmds.size() << endl;
		//cout << "Whats at c_c" << c_c << endl;

		//cout << "    DEBUG The first cmd is: " << allCmds[c_s] << endl;
		//cout << "    DEBUG What is success: " << succeeded << endl;
		//cout << "    DEBUG What is next conn: " << c_c << endl;
		//cout << "    DEBUG What is init " << init << endl;
		//cout << "    DEBUG " << (!succeeded && c_c == "||") << endl; 
		//cout << "    DEBUG What is success and c_c &&:" << (succeeded && c_c == "&&") << endl;
		//cout << "    DEBUG if statement evaluates: " << ((init || c_c == ";" || c_c == "end" || (succeeded && c_c == "&&") || 
		// (!succeeded && c_c == "||")) && allCmds[c_s] != "test") << endl;

		unsigned next_inx = 0;	
	        if((init || c_c == ";" || c_c == "end" || (succeeded && c_c == "&&") || 
		   (!succeeded && c_c == "||")) && allCmds[c_s] != "test"){    

		//cout << "    DEBUG WHY ARE WE IN HERE? " << endl; 
		  
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
		//Implies test was used
		else if(allCmds[c_s] == "test" ){
	        
  	        //cout << "test detected//////////////////////////////////////////" << endl;

		    init = 0;
		    //Set new connector
		    if(c_p < allCmds.size())
		        c_c = allCmds[c_p];
		    else
		        c_c = "end";
		   
		    //Check for flags. Careful about out of index!
		    //Flags are -e, -f, -d or no flag which evals to -e
		    //Args can or may not exist. Args can be one string only. Multiple
		    //Args will output an error and be counted as a failed execution

		    //c_p is the connector and c_s is the start.
		    //c_s can at most be one before the next connector c_s < c_p
		    //and c_s + n where n is the size-1 of this cmd sequence 
		    //must be < size of all Cmd sequences.
		    if(c_s + 1 < c_p && c_p - c_s <= 3){
			struct stat sb;
		
			if(c_s + 2 < c_p){
			    if(allCmds[c_s + 1] == "-e"){

			//cout << "    DEBUG entered e" << endl;

				stat(allCmds[c_s + 2].c_str(), &sb);
				if(S_ISREG(sb.st_mode) || S_ISDIR(sb.st_mode))
				    succeeded = 1;
				else
				    succeeded = 0;
			    }
			    else if(allCmds[c_s + 1] == "-f"){

			//cout << "    DEBUG entered f////////////////////////////////////////////" << endl;
			//cout << "    DEBUG what is allCmds at c_s + 2: " << allCmds[c_s + 2] << endl;
			//cout << "    DEBUG what is stat: " << stat(allCmds[c_s+2].c_str(), &sb) << endl;
stat(allCmds[c_s + 2].c_str(), &sb);

				if(S_ISREG(sb.st_mode)) 
				    succeeded = 1;
				else
				    succeeded = 0;
			    }	
			    else if(allCmds[c_s + 1] == "-d"){
stat(allCmds[c_s + 2].c_str(), &sb);
			//cout << "    DEBUG entered d" << endl;

				if(S_ISDIR(sb.st_mode)) 
				    succeeded = 1;
				else
				    succeeded = 0;

			    }
			}
			//Deafaults to -e
			else{
			
			//cout << "    DEBUG default e" << endl;

			    if(stat(allCmds[c_s + 1].c_str(), &sb)==-1){
				perror("Stat error");
			 	succeeded = 0;
			    }
			    else if((sb.st_mode & S_IFREG) || (sb.st_mode & S_IFDIR)){
				    succeeded = 1;
			
			//cout << "    DEBUG default e success" << endl;

			    }
			}
		    }
		    else{
			cout << "Bash: test error." << endl;
		        succeeded = 0; 
		    }
		
		    c_p++;
		    c_s = c_p;
		}
		//Implies failed cmd combo(eg. SUCCESS || CMD, CMD fails)
		else{
		    //cout << "Welcome to limbo" << endl << endl;
		    c_p++;
		    c_s = c_p;	
		    succeeded = 0;
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
