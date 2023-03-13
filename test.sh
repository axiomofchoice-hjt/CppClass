cd test
g++ Def.cc -c -Wall
g++ ../AxMarshal/Bin.cc -c -o Bin.o
g++ ../AxMarshal/Json.cc -c -o Json.o
g++ test.cc Def.o Bin.o Json.o -o test -Wall
./test