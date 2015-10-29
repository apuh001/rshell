all: rshell
	mkdir bin
	cd src
	mv rshell ../
	cd ..
	mv rshell bin

rshell: main.cp
	g++ -std=c++0x -Wall -Werror -ansi -pedantic main.cpp -o rshell
        	
clean:
	cd bin 
	rm rshell
	cd ..
	rmdir bin
