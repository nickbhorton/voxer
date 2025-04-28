eXX = g++
CXXFLAGS = -Wall -Werror -O3 -Isrc -Ideps/glad/include

all: render1 render2 render3 render4 render5 render6

render1: src/renderer1.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

render2: src/renderer2.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

render3: src/renderer3.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

render4: src/renderer4.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

render5: src/renderer5.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

render6: src/renderer6.cpp window.o glad.o buffer.o shader_program.o shader_object.o vao.o voxel_parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lglfw

window.o: src/window.cpp src/window.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

buffer.o: src/buffer.cpp src/buffer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

voxel_parser.o: src/voxel_parser.cpp src/voxel_parser.h
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
	rm -f render1
	rm *.o

.PHONY:
	clean
