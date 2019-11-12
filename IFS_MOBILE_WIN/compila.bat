del *.o
windres -o ico_res.o ico_res.rc
g++ -c mobile.cpp -O4
g++ mobile.o  ico_res.o -O4 -o mobile.exe -mwindows
del *.o