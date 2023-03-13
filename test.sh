cd test
g++ Def.cpp -c -Wall
g++ ../AxMarshal/Bin.cpp -c -o Bin.o
g++ ../AxMarshal/Json.cpp -c -o Json.o
g++ test.cpp Def.o Bin.o Json.o -o test -Wall
./test