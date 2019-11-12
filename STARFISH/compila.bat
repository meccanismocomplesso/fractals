del *.o
windres -o starfish_res.o starfish.rc
g++ -c *.cpp -O4
g++ winproc.o starfish.o options.o utils.o spline.o starfish_res.o -lscrnsave -O4 -o starfish.scr -mwindows
del *.o