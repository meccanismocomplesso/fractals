rem compiled on MinGW 3.2.0
del *.o
windres -o vertigo_res.o vertigo.rc
g++ -c *.cpp -O4
g++ winproc.o vertigo.o options.o utils.o vertigo_res.o -lscrnsave -O4 -o vertigo.scr -mwindows
del *.o