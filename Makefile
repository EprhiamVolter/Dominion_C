CXX = g++
CXXFLAGS = -fmax-errors=1 -Ih
SRC = $(wildcard cpp/*.cpp)
OBJ = $(SRC:cpp/%.cpp=o/%.o)
DEP = $(OBJ:%.o=%.d)

run: main.exe
	@./main.exe
	
main : main.exe

main.exe: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

o/%.o: cpp/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<
	
-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) main main.exe