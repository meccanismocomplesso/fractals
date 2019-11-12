del *.o
windres -o ico_res.o ico_res.rc
g++ -c inter.cpp -O4
g++ inter.o  ico_res.o -O4 -o inter.exe -mwindows
del *.o