del *.o
windres -o ico_res.o ico_res.rc
g++ -c newtonraphson.cpp -O4
g++ newtonraphson.o  ico_res.o -O4 -o newtonraphson.exe -mwindows
del *.o