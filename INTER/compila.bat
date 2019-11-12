rem compiled on WinGW 3.2.0
del *.o
windres -o inter_res.o inter.rc
g++ -c *.cpp -O4
g++ winproc.o inter.o options.o utils.o inter_res.o -lscrnsave -O4 -o inter.scr -mwindows
del *.o