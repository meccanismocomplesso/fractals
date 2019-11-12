del *.o
windres -o ico_res.o ico_res.rc
g++ -c moire.cpp -O4
g++ moire.o  ico_res.o -O4 -o moire.exe -mwindows
del *.o