CC = g++

all:	main.o
	$(CC) -o pry01.exe main.o

main.o:	src/main.cpp
	$(CC) -Iinc -O0 -g3 -Wall -c src/main.cpp
	$(CC) -Iinc -masm=intel -g3 -Wall -c src/main.cpp -S