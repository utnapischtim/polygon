WS_PORT =

# -pg removed, causing problems when compiling with g++-7.1.0
CPPFLAGS = -g \
           -std=c++1z \
           -fconcepts \
           -Wshadow \
           -Winit-self \
           -Wredundant-decls \
           -Wundef \
           -Wfloat-equal \
           -Wunreachable-code \
           -Weffc++ \
           -Wall \
           -pedantic \
           -Wfatal-errors \
           -Wextra

LDFLAGS = -lboost_system \
          -lpthread -DELP_THREAD_SAFE \
          lib/opencv2/libopencv_core.so.3.1.0

# -isystem instead of -I to supress warnings from this headers
INCLUDE_DIRS = -isystem./lib/json/src \
               -isystem./lib/easyloggingpp/src \
               -isystem./lib/websocketpp \
               -isystem./lib/

LIB_DIRS = -Llib/opencv2/ \
           -Wl,-rpath=lib/opencv2/

PROCESS = polygon
BIN_DIR = bin/
BUILD_DIR = build/
SRC_DIR = src/

FLYMAKE_TEMP_FILES = build/*flymake

EXE = $(BIN_DIR)$(PROCESS)

SRCS = $(filter-out $(shell find src/ -name '*.cpp' | sed 's/\(.*\).cpp/\1_flymake.cpp/'), $(wildcard src/*.cpp))
OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(SRCS)))))
DEPS = $(OBJS:.o=.d)



RM  = rm -f
CXX = g++-7.1.0

COMPILE := $(CXX) $(CPPFLAGS) $(INCLUDE_DIRS) -c
DEPEND  := $(CXX) -MM $(CPPFLAGS) $(INCLUDE_DIRS)
LINK    := $(CXX) $(CPPFLAGS)

CXX_COMPILER := $(shell command -v $(CXX) 2> /dev/null)
CONFIGURE_OPENCV2=cmake -DWITH_FFMPEG=OFF -DWITH_1394=OFF -DWITH_AVFFOUNDATION=OFF -DWITH_CARBON=OFF -DWITH_VTK=OFF -DWITH_EIGEN=ON -DWITH_VFW=OFF -DWITH_GSTREAMER=OFF -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_GTK_3_X=ON -DWITH_GPHOTO2=OFF -DWITH_MATLAB=OFF -DWITH_DIRECTX=OFF -DWITH_OPENCLAMBLAS=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_DSHOW=OFF -DLIBV4L=OFF -DWITH_V4L=OFF -DWITH_WIN32UI=OFF -DWITH_GIGEAPI=OFF -DWITH_PVAPI=OFF -DWITH_OPENEXER=OFF -DWITH_WEBP=OFF -DWITH_JASPER=OFF -DWITH_IPP=OFF -DENABLE_PRECOMPILED_HEADERS=OFF .

# Delete the default suffixes
.SUFFIXES:

.PHONY: all
all: $(EXE)

$(EXE): $(OBJS)
	@echo 'link the object files'
	@echo $(LINK) $^ -o $@ $(LIB_DIRS) $(LDFLAGS)
	@$(LINK) $^ -o $@ $(LIB_DIRS) $(LDFLAGS)

# help from
# http://scottmcpeak.com/autodepend/autodepend.html
$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@echo "build "$(PROCESS)" file: "$<
	@$(COMPILE) $< -o $@ $(LIB_DIRS)
	@$(DEPEND)	$< > $(BUILD_DIR)$*.d
	@mv -f $(BUILD_DIR)$*.d $(BUILD_DIR)$*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $(BUILD_DIR)$*.d.tmp > $(BUILD_DIR)$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILD_DIR)$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILD_DIR)$*.d
	@rm -f $(BUILD_DIR)$*.d.tmp

.PHONY: clean
clean:
	$(RM) $(DEPS) $(OBJS) $(EXE) $(FLYMAKE_TEMP_FILES)

check-syntax:
	$(RM) $(FLYMAKE_TEMP_FILES)
	$(CXX) $(INCLUDE_DIRS) -o /dev/null $(CPPFLAGS) -S $(CHK_SOURCES) $(LIB_DIRS) $(LDFLAGS)

.PHONY: server
server:
	@./$(EXE) $(WS_PORT) &

init: COMPILER
	@echo "init library"
	mkdir -p bin/
	mkdir -p build/
	mkdir -p lib/

# 9.89 is the last version of easyloggingpp as single header
	test ! -d lib/websocketpp && cd lib/ && git clone git://github.com/zaphoyd/websocketpp
	test ! -d lib/Catch && cd lib/ && git clone git://github.com/philsquared/Catch
	test ! -d lib/json && cd lib/ && git clone git://github.com/nlohmann/json
	test ! -d lib/easyloggingpp && cd lib/ && git clone git://github.com/easylogging/easyloggingpp && cd esayloggingpp/ && git checkout 9.89
	test ! -d lib/opencv2 && cd lib/ && git clone git://github.com/opencv/opencv && cd opencv/ && git checkout 3.2.0 && export CXX=$(CXX_COMPILER) && eval $(CONFIGURE_OPENCV2) && make

COMPILER:
ifndef CXX_COMPILER
		$(error $(CXX) does not exist)
endif

debugOut:
	@echo 'srcs: ' $(SRCS)
	@echo 'objs: ' $(OBJS)
	@echo 'deps: ' $(DEPS)
	@echo ''
	@echo 'compile:      ' $(COMPILE)
	@echo 'depend:       ' $(DEPEND)
	@echo 'link:         ' $(LINK)
	@echo 'CXX_COMPILER: ' $(CXX_COMPILER)
