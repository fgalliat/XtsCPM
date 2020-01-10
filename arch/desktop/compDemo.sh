clear

if [ "$1" = "arm" ]
then
 # Arietta version
 . ./arietta.arch
else
 # Desktop version
 . ./desktop.arch
fi

echo "Compiling subsystem DEMO [$ARCH version]"
rm demo 2>/dev/null
g++ -std=c++11 -Wno-write-strings -D$ARCH $DIRECTIVES -o demo $CPPs demo.cpp $LIBS 2>&1 
