del *.o
windres -o strangeflower_res.o strangeflower.rc
g++ -c *.cpp -O4
g++ winproc.o strangeflower.o options.o utils.o strangeflower_res.o -lscrnsave -O4 -o strangeflower.scr -mwindows
del *.o