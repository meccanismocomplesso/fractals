del *.o
windres -o ico_res.o ico_res.rc
g++ -c sierpinsky.cpp -O4
g++ sierpinsky.o  ico_res.o -O4 -o sierpinsky.exe -mwindows
del *.o