del *.o
windres -o ico_res.o ico_res.rc
g++ -c ifs.cpp -O4
g++ ifs.o  ico_res.o -O4 -o ifs.exe -mwindows
del *.o