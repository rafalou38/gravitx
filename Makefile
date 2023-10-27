CXX = ../../raylib/w64devkit/bin/g++
RAYLIB_PATH = ../../raylib/raylib
CXXFLAGS = -DPLATFORM_DESKTOP -static -std=c++20 -Wall

INCLUDE_PATHS = -I. -Iexternal -I$(RAYLIB_PATH)/src -Ilib -Ilib/imgui
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

SRCDIR = src
LIBDIR = lib
IMGUI_DIR = $(LIBDIR)/imgui
OBJDIR = obj

SOURCES = $(SRCDIR)/gravitx.cpp $(LIBDIR)/tinyxml2.cpp $(wildcard $(IMGUI_DIR)/*.cpp)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

EXECUTABLE = gravitx

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS)

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean
