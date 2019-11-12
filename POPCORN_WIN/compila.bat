del *.o
windres -o ico_res.o ico_res.rc
g++ -c popcorn.cpp -O4
g++ popcorn.o  ico_res.o -O4 -o popcorn.exe -mwindows
del *.o