all: synct.cpp
	make build
	make execute

build: synct.cpp
	g++ synct.cpp -o synct -pthread

execute: synct.exe
	synct.exe 200 1000

clean:
	del *.exe