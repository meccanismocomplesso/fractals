del *.o
windres -o resource.o resource.rc
g++ -c fractalizer.cpp
g++ -o fractalizer.exe fractalizer.o resource.o -O4 -mwindows
del *.o
