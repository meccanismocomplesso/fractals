rem compiled on MinGW 3.2.0
del *.o
windres -o hopalong_res.o hopalong.rc
g++ -c *.cpp -O4
g++ winproc.o hopalong.o options.o utils.o hopalong_res.o -lscrnsave -O4 -o hopalong.scr -mwindows
del *.o