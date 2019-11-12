del *.o
windres -o ico_res.o ico_res.rc
g++ -c mandel.cpp -O4
g++ mandel.o  ico_res.o -O4 -o mandel.exe -mwindows
del *.o