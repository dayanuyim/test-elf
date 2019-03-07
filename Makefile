all: elfparser.cpp elf_util.h
	g++ -std=c++17 -Wall -Wextra -pedantic -o elfparser elfparser.cpp

clean:
	rm -f elfparser
