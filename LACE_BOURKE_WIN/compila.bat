del *.o
windres -o ico_res.o ico_res.rc
g++ -c lace.cpp -O4
g++ lace.o  ico_res.o -O4 -o lace.exe -mwindows
del *.o