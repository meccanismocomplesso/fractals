del *.o
windres -o ico_res.o ico_res.rc
g++ -c *.cpp
g++ *.o ico_res.o -lopengl32 -lglaux -lglu32 -lscrnsave -o metaball.scr -mwindows
del *.o




