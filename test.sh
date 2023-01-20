cd test
g++ Def.cpp -c -Wall
g++ test.cpp Def.o -o test -Wall
./test