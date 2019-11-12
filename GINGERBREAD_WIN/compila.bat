del *.o
windres -o ico_res.o ico_res.rc
g++ -c gingerbread.cpp -O4
g++ gingerbread.o  ico_res.o -O4 -o gingerbread.exe -mwindows
del *.o