del *.o
windres -o ico_res.o ico_res.rc
g++ -c spiral.cpp -O4
g++ spiral.o  ico_res.o -O4 -o spiral.exe -mwindows
del *.o