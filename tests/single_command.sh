#!/bin/sh

cd ..
{
echo ls;
sleep 1;
echo "touch Hello";
sleep 1;
echo "ls";
echo "rm Hello";
sleep 3;
echo ls;
sleep 3;
echo "echo hi";
sleep 3;
echo "echo The next command I will type is: 0000. Execvp should fail it."
sleep 3;
echo 0000;
sleep 1;
echo "echo Quitting now."
sleep 1;
echo quit;
}| bin/rshell

