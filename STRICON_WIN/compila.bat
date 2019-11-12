del *.o
windres -o ico_res.o ico_res.rc
g++ -c stricon.cpp -O4
g++ stricon.o  ico_res.o -O4 -o stricon.exe -mwindows
del *.o