all: synct.cpp
	make build
	make execute

build: synct.cpp
	g++ synct.cpp -o synct -pthread

execute: synct.exe
	synct.exe

clean:
	del *.exe