all: synct.cpp
	make build
	make launch

build: synct.cpp
	g++ synct.cpp -o synct -pthread

launch: synct.exe
	./synct 200 1000

clean:
	del *.exe