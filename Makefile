all: rshell

rshell: main.cpp
	mkdir bin
	cd src
	g++ -std=c++0x -Wall -Werror -ansi -pedantic main.cpp -o rshell
	

clean:
	cd bin 
	rm rshell
