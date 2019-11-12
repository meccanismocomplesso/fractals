del *.o
windres -o stricon_res.o stricon.rc
g++ -c *.cpp -O4
g++ winproc.o stricon.o options.o utils.o stricon_res.o -lscrnsave -O4 -o stricon.scr -mwindows
del *.o