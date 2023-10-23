# Build mode for project: DEBUG or RELEASE
BUILD_MODE         ?= DEBUG
PLATFORM           ?= PLATFORM_DESKTOP

RAYLIB_PATH        ?= ../../raylib/raylib
COMPILER_PATH      ?= ../../raylib/w64devkit/bin

CC = ../../raylib/w64devkit/bin/g++


CFLAGS += -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces $(RAYLIB_PATH)/src/raylib.rc.data

# ifeq ($(BUILD_MODE),DEBUG)
CFLAGS += -g# -O0
# else
#     CFLAGS += -s -O1
# endif

INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

OBJS = obj/gravitx.o obj/tinyxml2.o

all: gravitx
gravitx: $(OBJS)
	@echo "Linking gravitx"
	$(CC) -o gravitx.exe $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

obj/%.o: %.cpp
	@echo "Compiling $<"
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)