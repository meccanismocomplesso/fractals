del *.o
windres -o ico_res.o ico_res.rc
g++ -c lhopalong.cpp -O4
g++ lhopalong.o  ico_res.o -O4 -o lhopalong.exe -mwindows
del *.o