main.exe : main.o resourse.o
	g++ $^ -o $@

resourse.o : resourse.rc
	windres -i $^ -o $@

main.o : main.cpp
	g++ $^ -o $@

clean:
	del *.exe *.o