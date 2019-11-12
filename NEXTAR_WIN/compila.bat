del *.o
windres -o ico_res.o ico_res.rc
g++ -c nextar.cpp -O4  
g++ nextar.o  ico_res.o -O4 -lopengl32 -lglaux -lglu32 -o nextar.exe -mwindows
del *.o