rem compiled on MinGW 3.2.0
del *.o
windres -o imsmap_res.o imsmap.rc
g++ -c *.cpp -O4
g++ winproc.o imsmap.o options.o utils.o imsmap_res.o -lscrnsave -O4 -o imsmap.scr -mwindows
del *.o