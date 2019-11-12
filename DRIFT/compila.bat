del *.o
windres -o drift_res.o drift.rc
g++ -c *.cpp -O4
g++ winproc.o drift.o options.o utils.o drift_res.o -lscrnsave -O4 -o drift.scr -mwindows
del *.o