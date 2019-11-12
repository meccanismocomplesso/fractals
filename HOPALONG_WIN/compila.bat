del *.o
windres -o ico_res.o ico_res.rc
g++ -c *.cpp -O4
g++ hopalong.o utils.o ico_res.o -O4 -o hopalong.exe -mwindows
del *.o