######
##
##  SETTINGS SECTION
##
#######

PROGRAM = polygon
TEST = tests
BIN_DIR = bin/
BUILD_DIR = build/
LIB_DIR = lib/
SRC_DIR = src/
TEST_DIR = unittests/

FLYMAKE_TEMP_FILES = build/*flymake

RM = rm -f
CXX = g++-8.1.0

WS_PORT =

# this could be overriden with make DEFS+=MAGICK to add automation
# output possibility
DEFS =
DEFINED = $(addprefix -D, ${DEFS})

# the problem is, with this step is, the warnings for the magick
# headers were displayed, but those are useless for this project
# IM_CPPFLAGS = `Magick++-config --cxxflags`

IM_CPPFLAGS = -DMAGICKCORE_HDRI_ENABLE=0 \
              -DMAGICKCORE_QUANTUM_DEPTH=16 \
              -isystem/usr/include/ImageMagick-7

IM_LDFLAGS = `Magick++-config --ldflags`

# -pg removed, causing problems when compiling with g++-7.1.0
CPPFLAGS = $(CFLAGS) -g \
           -std=c++2a \
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
           -Wextra \
           $(IM_CPPFLAGS)

LDFLAGS = -lboost_system \
          -lboost_iostreams \
          -lstdc++fs \
          -lpthread -DELP_THREAD_SAFE \
          -lCGAL \
          -lgmp \
          -ldocopt \
          $(IM_LDFLAGS)

ifeq ($(shell which $(CXX)),)
  $(error $(CXX) does not exist)
endif

ifeq ($(wildcard $(BIN_DIR)),)
  mkdir -p $(BIN_DIR)
endif

ifeq ($(wildcard $(BUILD_DIR)),)
  mkdir -p $(BUILD_DIR)
endif

ifeq ($(wildcard $(LIB_DIR)),)
  mkdir -p $(LIB_DIR)
endif

# if directories are created by the dependencies script the local version is
# used, otherwise the global installed version is used

ifneq ($(wildcard ./lib/Catch),)
  INCLUDE_DIRS += -isystem./lib/Catch/single_include
endif

ifneq ($(wildcard ./lib/gnuplot-iostream),)
  INCLUDE_DIRS += -isystem./lib/gnuplot-iostream
endif

ifneq ($(wildcard ./lib/json),)
  INCLUDE_DIRS += -isystem./lib/json/src
endif

ifneq ($(wildcard ./lib/easyloggingpp/src),)
  INCLUDE_DIRS += -isystem./lib/easyloggingpp/src
endif

ifneq ($(wildcard ./lib/websocketpp),)
  INCLUDE_DIRS += -isystem./lib/websocketpp
endif

ifneq ($(wildcard ./lib/docopt),)
  INCLUDE_DIRS += -isystem./lib/docopt

  LIB_DIRS = -Llib/docopt \
             -Wl,-rpath=lib/docopt
endif

ifneq ($(wildcard ./src),)
  INCLUDE_DIRS += -isystem./src
endif

COMPILE := $(CXX) $(CPPFLAGS) $(INCLUDE_DIRS) -c
DEPEND  := $(CXX) -MM $(CPPFLAGS) $(INCLUDE_DIRS)
LINK    := $(CXX) $(CPPFLAGS)

# Delete the default suffixes
.SUFFIXES:


######
##
##  PROGRAM SECTION
##
#######

EXE  = $(BIN_DIR)$(PROGRAM)
SRCS = $(filter-out $(shell find src/ -name '*flymake*.cpp'), $(wildcard src/*.cpp))
OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(SRCS)))))
DEPS = $(OBJS:.o=.d)


.PHONY: all
all: $(EXE)

$(EXE): $(OBJS)
	@echo 'link the object files'
	@$(LINK) $^ -o $@ $(LIB_DIRS) $(LDFLAGS)

# help from
# http://scottmcpeak.com/autodepend/autodepend.html
$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@echo "build "$(PROGRAM)" file: "$<
	@$(COMPILE) $< -o $@ $(LIB_DIRS) $(DEFINED)
	@$(DEPEND)	$< > $(BUILD_DIR)$*.d
	@mv -f $(BUILD_DIR)$*.d $(BUILD_DIR)$*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $(BUILD_DIR)$*.d.tmp > $(BUILD_DIR)$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILD_DIR)$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILD_DIR)$*.d
	@rm -f $(BUILD_DIR)$*.d.tmp

.PHONY: clean
clean:
	$(RM) $(DEPS) $(OBJS) $(EXE) $(FLYMAKE_TEMP_FILES)


######
##
##  SERVER SECTION
##
#######

.PHONY: server
server:
	@./$(EXE) $(WS_PORT) &


######
##
##  TEST SECTION
##
#######

TEST_EXE  = $(BIN_DIR)$(TEST)
TEST_SRCS = $(filter-out $(shell find unittests -name '*.cpp' | sed 's/\(.*\).cpp/\1_flymake.cpp/'), $(wildcard unittests/*.cpp)) src/Point.cpp
TEST_OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(TEST_SRCS)))))
TEST_DEPS = $(TEST_OBJS:.o=.d)

run-test: build-test
	$(TEST_EXE)

build-test: $(TEST_EXE)

clean-test:
	$(RM) $(TEST_DEPS) $(TEST_OBJS) $(TEST_EXE) $(FLYMAKE_TEMP_FILES)

$(TEST_EXE): $(TEST_OBJS)
	@$(LINK) $^ -o $@ $(LIB_DIRS) $(LDFLAGS) build/Filter.o build/utilities.o build/random.o build/CommonSetting.o build/Output.o build/SamplingGrid.o

$(BUILD_DIR)%.o: $(TEST_DIR)%.cpp
	@echo "build "$(PROGRAM)" file: "$<
	@$(COMPILE) $< -o $@ $(LIB_DIRS)
	@$(DEPEND)	$< > $(BUILD_DIR)$*.d
	@mv -f $(BUILD_DIR)$*.d $(BUILD_DIR)$*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $(BUILD_DIR)$*.d.tmp > $(BUILD_DIR)$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILD_DIR)$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILD_DIR)$*.d
	@rm -f $(BUILD_DIR)$*.d.tmp


######
##
##  EMACS SECTION
##
#######

# emacs help rule to process file within emacs and return the error messages
check-syntax:
	$(RM) $(FLYMAKE_TEMP_FILES)
	$(CXX) $(INCLUDE_DIRS) -o /dev/null $(CPPFLAGS) -S $(CHK_SOURCES) $(LIB_DIRS) $(LDFLAGS)


######
##
##  DEBUG SECTION
##
#######

debugOut:
	@echo 'srcs: ' $(SRCS)
	@echo 'objs: ' $(OBJS)
	@echo 'deps: ' $(DEPS)
	@echo ''
	@echo 'compile:      ' $(COMPILE)
	@echo 'depend:       ' $(DEPEND)
	@echo 'link:         ' $(LINK)
	@echo 'CXX_COMPILER: ' $(CXX_COMPILER)
	@echo 'CPPFLAGS: ' $(CPPFLAGS)
	@echo 'LDFLAGS:  ' $(LDFLAGS)


