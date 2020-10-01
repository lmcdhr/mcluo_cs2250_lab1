all:	
	g++ -std=c++11 -c main.cpp
	g++ -std=c++11 -c Module.cpp
	g++ -std=c++11 -c Instruction.cpp
	g++ -std=c++11 -o main main.o Module.o Instruction.o

clean:

	rm *.o
	rm main
