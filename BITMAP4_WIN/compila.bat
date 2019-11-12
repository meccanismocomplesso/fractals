del *.o
windres -o ico_res.o ico_res.rc
g++ -c *.cpp -O4
g++ *.o -O4 -o bitmap4.exe -mwindows
del *.o