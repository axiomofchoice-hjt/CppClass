cd test
g++ Def.cpp -c -Wall
g++ ../CppClass/Bin.cpp -c -o Bin.o
g++ ../CppClass/Json.cpp -c -o Json.o
g++ test.cpp Def.o Bin.o Json.o -o test -Wall
./test