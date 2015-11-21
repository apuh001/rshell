#!/bin/sh

cd ..

{
echo "echo creating a file called f.txt";
echo "touch f.txt;"
sleep 3;
echo "echo Trying: test f.txt || echo nothing should print";
sleep 2;
echo "test f.txt || echo nothing should print";
sleep 2;
echo "echo "Trying: test -d f.txt || echo should fail"";
sleep 2;
echo "test -d f.txt || echo should fail";
sleep 2;
echo "echo "Trying: test -e f.txt && echo should work"";
sleep 2;
echo "test -e f.txt && echo should work";
sleep 2;
echo "echo "Making a new directory: someDir"";
sleep 2;
echo "mkdir someDir";
sleep 2;
echo "echo "Trying [ someDir ] && echo should work"";
sleep 2;
echo "[ someDir ] && echo should work";
sleep 2;
echo "echo "Trying [ -f someDir ] || echo should fail"";
sleep 2;
echo "[ -f someDir ] || echo should fail";
sleep 2;
echo "rm f.txt;"
sleep 2;
echo "rmdir someDir;"
sleep 2;
echo  "echo Quitting now."
sleep 2;
echo quit;
}| bin/rshell
