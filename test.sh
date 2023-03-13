./build/axmc/axmc ./test/Def.txt ./test
cp axm test -rf
cd test
rm build -rf
mkdir build
cd build
cmake .. && make -j && ./test
