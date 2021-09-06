# release build - make DEBUG=0
# debug build (default) - make DEBUG=1

DEBUG  ?= 1
WEB  ?= 0


RM=rm -f

PCH_I=./source/Project.h
PCH_O=$(PCH_I).pch
PCH_C=-include $(PCH_I)

CPPFLAGS=-std=c++17
LDLIBS=-lraylib 

ifeq ($(WEB), 1)
	CXX=emcc

	CPPFLAGS+=-I./vendor/raylib/webassembly
	LDFLAGS+=-L./vendor/raylib/webassembly

# -Os                        # size optimization
# -O2                        # optimization level 2, if used, also set --memory-init-file 0
# -s USE_GLFW=3              # Use glfw3 library (context/input management)
# -s ALLOW_MEMORY_GROWTH=1   # to allow memory resizing -> WARNING: Audio buffers could FAIL!
# -s TOTAL_MEMORY=16777216   # to specify heap memory size (default = 16MB) (67108864 = 64MB)
# -s USE_PTHREADS=1          # multithreading support
# -s WASM=0                  # disable Web Assembly, emitted by default
# -s ASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
# -s FORCE_FILESYSTEM=1      # force filesystem to load/save files data
# -s ASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
# --profiling                # include information for code profiling
# --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
# --preload-file resources   # specify a resources folder for data compilation
# --source-map-base          # allow debugging in browser with source map
#CPPFLAGS+=-s USE_PTHREADS=1
#LDFLAGS+=-s USE_GLFW=3 -s WASM=1 -s ASYNCIFY -s ASSERTIONS=1 --source-map-base simon.map
	CPPFLAGS+=-DPLATFORM_WEB

	LDFLAGS+=-s USE_GLFW=3
	LDFLAGS+=--preload-file button_menu.png
	LDFLAGS+=--preload-file button_play.png
	LDFLAGS+=--preload-file button_restart.png
	LDFLAGS+=--preload-file button_resume.png
	OUT_EXT=.html

else
	CXX=clang++
	CPPFLAGS+=-stdlib=libc++

	CPPFLAGS+=-I./vendor/raylib/mac
	LDFLAGS+=-L./vendor/raylib/mac

	LDLIBS+=-lc++ -framework OpenGL -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT
endif


ifeq ($(DEBUG), 1)
	CPPFLAGS +=-g -Wall -pedantic
	LDFLAGS +=-g
else
	CPPFLAGS += -O3 -DNDEBUG
endif



SRCS=$(shell find ./source -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

TEST_SRCS=$(shell find ./test -name "*.cpp")
TEST_OBJS=$(subst .cpp,.o,$(TEST_SRCS))

PCH_HEADERS=$(shell find ./source -name "*.h")

all: imSquared

imSquared: $(OBJS) ./main.cpp
	$(CXX) $(CPPFLAGS) $(PCH_C) $(LDFLAGS) -o imSquared$(OUT_EXT) $^ $(LDLIBS)

# pch compile rule
# make the pch depend on all header
# changing an header triggers a full rebuild
$(PCH_O): $(PCH_I) $(PCH_HEADERS)
	$(CXX) $(CPPFLAGS) -x c++-header $<  -o $@ 

%.o : %.cpp $(PCH_O) 
	$(CXX) -c $(CPPFLAGS) $(PCH_C) $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(TEST_OBJS)
	$(RM) $(PCH_O)
	$(RM) imSquared imSquared.data imSquared.wasm imSquared.js imSquared.html imSquared.map
	$(RM) -rf *.dSYM
	