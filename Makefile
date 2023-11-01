CXX = ../../raylib/w64devkit/bin/g++
RAYLIB_PATH = ../../raylib/raylib
CXXFLAGS = -DPLATFORM_DESKTOP -static -std=c++20 -Wall


CXXFLAGS += -g
# DEBUG = true

INCLUDE_PATHS = -I. -IeUGxternal -I$(RAYLIB_PATH)/src -Ilib -Ilib/imgui -I$(RAYLIB_PATH)/src/external/glfw/include
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

SRCDIR = src
LIBDIR = lib
IMGUI_DIR = $(LIBDIR)/imgui
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(LIBDIR)/tinyxml2.cpp $(wildcard $(IMGUI_DIR)/*.cpp) $(LIBDIR)/rlImGui/rlImGui.cpp
HEADERS = $(wildcard $(SRCDIR)/*.hpp)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

EXECUTABLE = gravitx

export PATH := $(RAYLIB_PATH)\..\w64devkit\bin;$(PATH)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking $(EXECUTABLE)..."
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS)
	@echo "$(EXECUTABLE) has been built."

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/imgui/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/rlImGui/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(EXECUTABLE)
	@echo "Cleaned."

.PHONY: all clean
