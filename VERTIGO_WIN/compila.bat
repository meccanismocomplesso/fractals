del *.o
windres -o ico_res.o ico_res.rc
g++ -c *.cpp -O4
g++ vertigo.o utils.o ico_res.o -O4 -o vertigo.exe -mwindows
del *.o