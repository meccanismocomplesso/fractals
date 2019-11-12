del *.o
windres -o ico_res.o ico_res.rc
g++ -c quadruptwo.cpp -O4
g++ quadruptwo.o  ico_res.o -O4 -o quadruptwo.exe -mwindows
del *.o