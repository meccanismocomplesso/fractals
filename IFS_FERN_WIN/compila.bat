del *.o
windres -o ico_res.o ico_res.rc
g++ -c fern.cpp -O4
g++ fern.o  ico_res.o -O4 -o fern.exe -mwindows
del *.o