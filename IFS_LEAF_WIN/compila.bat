del *.o
windres -o ico_res.o ico_res.rc
g++ -c leaf.cpp -O4
g++ leaf.o  ico_res.o -O4 -o leaf.exe -mwindows
del *.o