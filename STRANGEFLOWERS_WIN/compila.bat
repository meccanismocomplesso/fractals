del *.o
windres -o ico_res.o ico_res.rc
g++ -c strangeflower.cpp -O4
g++ strangeflower.o  ico_res.o -O4 -o strangeflower.exe -mwindows
del *.o