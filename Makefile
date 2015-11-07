all: rshell bin

bin:
	mkdir bin
	mv rshell bin

rshell: src/rshell.cpp
	g++ -std=c++0x -Wall -Werror -ansi -pedantic src/rshell.cpp -o rshell
        	
clean:
	rm bin/rshell
	rmdir bin
