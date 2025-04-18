CXX = g++
CXXFLAGS = -std=c++11 -w
LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa -framework OpenAL

TARGET = flappy_bird
SRC = flappy_bird.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

