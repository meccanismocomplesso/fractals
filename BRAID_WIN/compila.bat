del *.o
windres -o ico_res.o ico_res.rc
g++ -c braid.cpp -O4
g++ braid.o  ico_res.o -O4 -o braid.exe -mwindows
del *.o