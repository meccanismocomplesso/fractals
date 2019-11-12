del *.o
windres -o resource.o resource.rc
g++ -c *.cpp
g++ -o IFSDrawer.exe *.o -O4 -mwindows
del *.o
