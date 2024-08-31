MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

CXX = g++
RAYLIB_PATH = /home/rafael/src/lib/raylib
CXXFLAGS = -DPLATFORM_DESKTOP -std=c++20 -Wall


CXXFLAGS += -ggdb3
# DEBUG = true

INCLUDE_PATHS = -I. -Ilib/imgui -Ilib/rlImGui -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -I/usr/local/include -I$(RAYLIB_PATH)/src/external/glfw/include # -I/usr/lib/x86_64-linux-gnu -I/usr/lib -I/usr/local/include -I$(RAYLIB_PATH)/src -Ilib  -I$(RAYLIB_PATH)/src/external/glfw/include
LDFLAGS = -L. -L$(RAYLIB_PATH)/src -L$(RAYLIB_PATH)/src -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic # -L/usr/lib/x86_64-linux-gnu -L/usr/lib -L/usr/local/lib  -lm -lpthread -ldl -lrt -lX11

SRCDIR = src
LIBDIR = lib
IMGUI_DIR = $(LIBDIR)/imgui
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(LIBDIR)/tinyxml2.cpp $(wildcard $(IMGUI_DIR)/*.cpp) $(LIBDIR)/rlImGui/rlImGui.cpp
HEADERS = $(wildcard $(SRCDIR)/*.hpp)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

EXECUTABLE = gravitx

# export PATH := $(RAYLIB_PATH)\..\w64devkit\bin;$(PATH)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
# @echo "Linking $(EXECUTABLE)..."
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS)
# @echo "$(EXECUTABLE) has been built."

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(OBJDIR)/%.o: lib/rlImGui/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(EXECUTABLE)
# @echo "Cleaned."

.PHONY: all clean
