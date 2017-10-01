
clang -c -o pr.o pr.c

clang++ -O3 -Xclang -load -Xclang ./libp1.so pr.o test.cc


