cd test
g++ Def.cpp -c -Wall
g++ ../CppClass.cpp -c -o CppClass.o
g++ test.cpp Def.o CppClass.o -o test -Wall
./test