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

SRCS = $(wildcard src/*.cpp)
OBJS = $(addprefix $(BUILD_DIR), $(addsuffix .o, $(notdir $(basename $(SRCS)))))
DEPS = $(OBJS:.o=.d)



RM  = rm -f
CXX = g++-7.1.0

COMPILE := $(CXX) $(CPPFLAGS) $(INCLUDE_DIRS) -c
DEPEND  := $(CXX) -MM $(CPPFLAGS) $(INCLUDE_DIRS)
LINK    := $(CXX) $(CPPFLAGS)

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

init:
	@echo "init library"
	mkdir -p bin/
	mkdir -p build/
	mkdir -p lib/

	test ! -d lib/Catch && cd lib/ && git clone git://github.com/philsquared/Catch
	test ! -d lib/websocketpp && cd lib/ && git clone git://github.com/zaphoyd/websocketpp
	test ! -d lib/json && cd lib/ && git clone git://github.com/nlohmann/json
	test ! -d lib/easyloggingpp && cd lib/ && git clone git://github.com/easylogging/easyloggingpp

debugOut:
	@echo 'srcs: ' $(SRCS)
	@echo 'objs: ' $(OBJS)
	@echo 'deps: ' $(DEPS)
	@echo ''
	@echo 'compile: ' $(COMPILE)
	@echo 'depend:  ' $(DEPEND)
	@echo 'link:    ' $(LINK)
