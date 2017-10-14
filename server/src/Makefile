IM_CPPFLAGS = -fopenmp \
              -DMAGICKCORE_HDRI_ENABLE=0 \
              -DMAGICKCORE_QUANTUM_DEPTH=16 \
              -isystem/usr/include/ImageMagick-7

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
           -Wextra \
           $(IM_CPPFLAGS)

LDFLAGS =

# -isystem instead of -I to supress warnings from this headers
INCLUDE_DIRS = -isystem../lib/json/src \
               -isystem../lib/gnuplot-iostream \
               -isystem../lib/easyloggingpp/src \
               -isystem../lib/websocketpp \
               -isystem../lib/docopt \
               -isystem../lib/

LIB_DIRS =

FLYMAKE_TEMP_FILES = build/

RM  = rm -f
CXX = g++-7.1.0

check-syntax:
	$(RM) $(FLYMAKE_TEMP_FILES)
	$(CXX) $(INCLUDE_DIRS) -o /dev/null $(CPPFLAGS) -S $(CHK_SOURCES) $(LIB_DIRS) $(LDFLAGS)
