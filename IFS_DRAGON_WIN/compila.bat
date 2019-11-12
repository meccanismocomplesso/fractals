del *.o
windres -o ico_res.o ico_res.rc
g++ -c dragon.cpp -O4
g++ dragon.o  ico_res.o -O4 -o dragon.exe -mwindows
del *.o