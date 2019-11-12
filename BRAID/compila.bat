rem compilate on MinGW 3.2.0
del *.o
windres -o braid_res.o braid.rc
g++ -c *.cpp -O4
g++ winproc.o braid.o options.o utils.o braid_res.o -lscrnsave -O4 -o braid.scr -mwindows
del *.o