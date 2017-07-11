WS_PORT =

OPENCV_VERSION=3.2.0

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
          lib/opencv2/libopencv_core.so.$(OPENCV_VERSION)

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
CONFIGURE_OPENCV2=cmake -DWITH_1394=OFF -DWITH_AVFOUNDATION=OFF -DWITH_CARBON=OFF -DWITH_CAROTENE=OFF -DWITH_CPUFEATURES=OFF -DWITH_VTK=OFF -DWITH_CUDA=OFF -DWITH_CUFFT=OFF -DWITH_CUBLAS=OFF -DWITH_NVCUVID=OFF -DWITH_EIGEN=OFF -DWITH_VFW=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_GSTREAMER_0_10=OFF -DWITH_GTK=OFF -DWITH_GTK_2_X=OFF -DWITH_IPP=OFF -DWITH_HALIDE=OFF -DWITH_JASPER=OFF -DWITH_JPEG=OFF -DWITH_WEBP=OFF -DWITH_OPENEXR=OFF -DWITH_OPENGL=OFF -DWITH_OPENVX=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_PNG=OFF -DWITH_GDCM=OFF -DWITH_PVAPI=OFF -DWITH_GIGEAPI=OFF -DWITH_ARAVIS=OFF -DWITH_QT=OFF -DWITH_WIN32UI=OFF -DWITH_QUICKTIME=OFF -DWITH_QTKIT=OFF -DWITH_TBB=OFF -DWITH_OPENMP=OFF -DWITH_CSTRIPES=OFF -DWITH_PTHREADS_PF=OFF -DWITH_TIFF=OFF -DWITH_UNICAP=OFF -DWITH_V4L=OFF -DWITH_LIBV4L=OFF -DWITH_DSHOW=OFF -DWITH_MSMF=OFF -DWITH_XIMEA=OFF -DWITH_XINE=OFF -DWITH_CLP=OFF -DWITH_OPENCL=OFF -DWITH_OPENCL_SVM=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_DIRECTX=OFF -DWITH_INTELPERC=OFF -DWITH_IPP_A=OFF -DWITH_MATLAB=OFF -DWITH_VA=OFF -DWITH_VA_INTEL=OFF -DWITH_MFX=OFF -DWITH_GDAL=OFF -DWITH_GPHOTO2=OFF -DWITH_LAPACK=OFF -DWITH_ITT=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_opencv_apps=OFF -DBUILD_ANDROID_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PACKAGE=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DBUILD_WITH_DEBUG_INFO=OFF -DBUILD_WITH_STATIC_CRT=OFF -DBUILD_WITH_DYNAMIC_IPP=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_ANDROID_SERVICE=OFF -DBUILD_CUDA_STUBS=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_java=OFF -DBUILD_opencv_calib3d=OFF -DBUILD_opencv_cudaarithm=OFF -DBUILD_opencv_cudabgsegm=OFF -DBUILD_opencv_cudacodec=OFF -DBUILD_opencv_cudafeatures2d=OFF -DBUILD_opencv_cudafilters=OFF -DBUILD_opencv_cudaimgproc=OFF -DBUILD_opencv_cudalegacy=OFF -DBUILD_opencv_cudaobjdetect=OFF -DBUILD_opencv_cudaoptflow=OFF -DBUILD_opencv_cudastereo=OFF -DBUILD_opencv_cudawarping=OFF -DBUILD_opencv_cudev=OFF -DBUILD_opencv_features2d=OFF -DBUILD_opencv_flann=OFF -DBUILD_opencv_highgui=OFF -DBUILD_opencv_imgcodecs=OFF -DBUILD_opencv_imgproc=OFF -DBUILD_opencv_java=OFF -DBUILD_opencv_ml=OFF -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_photo=OFF -DBUILD_opencv_python=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_shape=OFF -DBUILD_opencv_stitching=OFF -DBUILD_opencv_superres=OFF -DBUILD_opencv_ts=OFF -DBUILD_opencv_video=OFF -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF -DBUILD_opencv_viz=OFF -DBUILD_opencv_world=OFF

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
	test ! -d lib/easyloggingpp && cd lib/ && git clone git://github.com/easylogging/easyloggingpp && cd easyloggingpp/ && git checkout 9.89
	test ! -d lib/opencv2 && cd lib/ && git clone git://github.com/opencv/opencv && cd opencv/ && git checkout $(OPENCV_VERSION) && export CXX=$(CXX_COMPILER) && eval $(CONFIGURE_OPENCV2) && make && mkdir ../opencv2 && cp -r modules/core/include/opencv2/core/ ../opencv2/ && cp modules/core/include/opencv2/core.hpp ../opencv2/core.hpp && cp lib/libopencv_core.so.$(OPENCV_VERSION) ../opencv2/libopencv_core.so.$(OPENCV_VERSION)

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
