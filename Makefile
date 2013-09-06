##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf
CMAKE_ARGS := -D CMAKE_CXX_FLAGS="-I/code/work/cyril/usr/local/include" -D CMAKE_EXE_LINKER_FLAGS="-L/code/work/cyril/usr/local/lib64" -D OPENIMAGEIO_LIBRARY="/code/tpl/el6/gcc-4.1.2/openimageio/0.11.0_2011-11-28_pf/lib/libOpenImageIO.so/" -D OPENIMAGEIO_INCLUDE_DIR="/code/tpl/el6/gcc-4.1.2/openimageio/0.11.0_2011-11-28_pf/include/"
CMAKE_CMD := cmake $(CMAKE_ARGS) ..

all: ./build/Makefile
	@ $(MAKE) -C build

./build/Makefile:
	@ (cd build >/dev/null 2>&1 && cmake ..)

distclean:
	@- (cd build >/dev/null 2>&1 && $(CMAKECMD) )
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build


ifeq ($(findstring distclean,$(MAKECMDGOALS)),)

    $(MAKECMDGOALS): ./build/Makefile
	@ $(MAKE) -C build $(MAKECMDGOALS)

endif
