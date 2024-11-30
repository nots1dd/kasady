# Compiler and tools
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2

# Include paths
INCLUDES = -I. -I/path/to/imgui -I/path/to/glfw/include $(shell pkg-config --cflags poppler-cpp)

# Libraries and linker options
LIBS = -lGL -lglfw -ldl -pthread -limgui $(shell pkg-config --libs poppler-cpp)
LDFLAGS =

# Source files
SOURCES = main.cpp backends/src/imgui_impl_glfw.cpp backends/src/imgui_impl_opengl3.cpp doc_viewer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Output binary
TARGET = kasady

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET)
