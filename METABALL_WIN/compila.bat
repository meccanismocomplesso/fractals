del *.o
windres -o ico_res.o ico_res.rc
g++ -c metaballs.cpp -O4  
g++ metaballs.o  ico_res.o -O4 -lopengl32 -lglaux -lglu32 -o metaballs.exe -mwindows
del *.o