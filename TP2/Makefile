CXX = g++
CXXFLAGS = `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

all: Tp

Tp: main.o camera.o
	$(CXX) -o Tp main.o camera.o $(LDFLAGS)

main.o: main.cpp
	$(CXX) -c main.cpp $(CXXFLAGS)

camera.o: camera.cpp
	$(CXX) -c camera.cpp $(CXXFLAGS)

clean:
	rm -f *.o Tp
