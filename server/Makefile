WS_PORT =

OPENCV_VERSION=3.2.0

# -pg removed, causing problems when compiling with g++-7.1.0
CPPFLAGS = $(CFLAGS) -g \
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
          -lstdc++fs \
          -lpthread -DELP_THREAD_SAFE \
          -lCGAL \
          -lgmp \
          -ldocopt

# -isystem instead of -I to supress warnings from this headers
INCLUDE_DIRS = -isystem./lib/Catch/single_include \
               -isystem./lib/json/src \
               -isystem./lib/easyloggingpp/src \
               -isystem./lib/websocketpp \
               -isystem./lib/docopt \
               -isystem./src/

LIB_DIRS = -Llib/docopt \
           -Wl,-rpath=lib/docopt

PROCESS = polygon
BIN_DIR = bin/
BUILD_DIR = build/
SRC_DIR = src/
TEST_DIR = unittests/

FLYMAKE_TEMP_FILES = build/*flymake

EXE = $(BIN_DIR)$(PROCESS)

SRCS = $(filter-out $(shell find src/ -name '*flymake*.cpp'), $(wildcard src/*.cpp))
OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(SRCS)))))
DEPS = $(OBJS:.o=.d)

TEST_EXE  = $(BIN_DIR)tests
TEST_SRCS = $(filter-out $(shell find unittests -name '*.cpp' | sed 's/\(.*\).cpp/\1_flymake.cpp/'), $(wildcard unittests/*.cpp))
TEST_OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(TEST_SRCS)))))
TEST_DEPS = $(TEST_OBJS:.o=.d)



RM  = rm -f
CXX = g++-7.1.0

COMPILE := $(CXX) $(CPPFLAGS) $(INCLUDE_DIRS) -c
DEPEND  := $(CXX) -MM $(CPPFLAGS) $(INCLUDE_DIRS)
LINK    := $(CXX) $(CPPFLAGS)

CXX_COMPILER := $(shell command -v $(CXX) 2> /dev/null)

# Delete the default suffixes
.SUFFIXES:

.PHONY: all
all: $(EXE)

$(EXE): $(OBJS)
	@echo 'link the object files'
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

$(BUILD_DIR)%.o: $(TEST_DIR)%.cpp
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
	test ! -d lib/easyloggingpp && cd lib/ && git clone git://github.com/easylogging/easyloggingpp && cd easyloggingpp/ && git checkout 9.89
	test ! -d lib/docopt && cd lib/ && git clone git://github.com/docopt/docopt.cpp docopt && cd docopt/ && cmake . && make

COMPILER:
ifndef CXX_COMPILER
		$(error $(CXX) does not exist)
endif

run-test:
	$(TEST_EXE)

build-test: $(TEST_EXE)

clean-test:
	$(RM) $(TEST_DEPS) $(TEST_OBJS) $(TEST_EXE) $(FLYMAKE_TEMP_FILES)

$(TEST_EXE): $(TEST_OBJS)
	@$(LINK) $^ -o $@ $(LIB_DIRS) $(LDFLAGS)

debugOut:
	@echo 'srcs: ' $(SRCS)
	@echo 'objs: ' $(OBJS)
	@echo 'deps: ' $(DEPS)
	@echo ''
	@echo 'compile:      ' $(COMPILE)
	@echo 'depend:       ' $(DEPEND)
	@echo 'link:         ' $(LINK)
	@echo 'CXX_COMPILER: ' $(CXX_COMPILER)
