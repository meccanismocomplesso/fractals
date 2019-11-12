del *.o
windres -o ico_res.o ico_res.rc
g++ -c twondragon.cpp -O4
g++ twondragon.o  ico_res.o -O4 -o twondragon.exe -mwindows
del *.o