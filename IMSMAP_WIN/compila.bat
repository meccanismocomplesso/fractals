del *.o
windres -o ico_res.o ico_res.rc
g++ -c imsmap.cpp -O4
g++ imsmap.o  ico_res.o -O4 -o imsmap.exe -mwindows
del *.o