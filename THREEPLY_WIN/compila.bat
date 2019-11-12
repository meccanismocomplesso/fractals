del *.o
windres -o ico_res.o ico_res.rc
g++ -c threeply.cpp -O4
g++ threeply.o  ico_res.o -O4 -o threeply.exe -mwindows
del *.o