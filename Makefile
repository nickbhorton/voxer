CXX = g++
CXXFLAGS = -Wall -Werror -g3 -Isrc -Ideps/glad/include

all: vp render

vp: src/voxel_parser.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

render: src/renderer.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

window.o: src/window.cpp src/window.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

buffer.o: src/buffer.cpp src/buffer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

vao.o: src/vao.cpp src/vao.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

shader_object.o: src/shader_object.cpp src/shader.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

shader_program.o: src/shader_program.cpp src/shader.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

glad.o: deps/glad/src/gl.c
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f vp
	rm -f render 
	rm *.o

.PHONY:
	clean
