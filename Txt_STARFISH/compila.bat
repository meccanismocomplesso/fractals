del *.o
g++ -c *.cpp -O4
g++ starfish.o options.o utils.o spline.o -O4 -o starfish.exe
del *.o