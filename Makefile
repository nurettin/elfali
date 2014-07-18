all:
	$(CXX) -std=c++1y main.cpp `pkg-config opencv --cflags --libs`
run:
	./a.out
