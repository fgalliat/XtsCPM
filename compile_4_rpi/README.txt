https://stackoverflow.com/questions/34807635/cross-compile-ncurses-application-for-arm-linux


http://ellcc.org/releases/release-2017-08-23/

~/ellcc/bin/ecc -target arm-linux-engeabihf -g -Wall -c main.c -o main.o 
~/ellcc/bin/ecc -target arm-linux-engeabihf -g -Wall main.o -o myApplication -lncurses 

It creates a static binary, so you don't have to worry about shared library versions.


https://solarianprogrammer.com/2017/12/08/raspberry-pi-raspbian-install-gcc-compile-cpp-17-programs/
