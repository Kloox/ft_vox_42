NAME = ft_vox
C_DIR = $(shell pwd)
GCCF = g++ -O6 -ffast-math -pthread -Wall -Wextra
GCCFGDB = g++ -ffast-math -pthread -g
GREEN       = \033[32m
RED         = \033[31m
DEFAULT     = \033[37m
PATHOBJ = obj/
PATHSRC = src/
DEP = Makefile ./src/Classes/Cpu.hpp ./includes/vox.hpp ./src/Classes/Engine.hpp ./src/Classes/World.hpp ./src/Classes/Chunk.hpp ./src/Classes/Noiser.hpp ./libs/irrKlang-64bit-1.6.0/include/irrKlang.h
SRC = vox.cpp textures.cpp utils.cpp Classes/Engine.cpp Classes/World.cpp Classes/Chunk.cpp Classes/Noiser.cpp
HEAD = -I ./includes/ -I ./includes/freetype2 -I ./src/Classes/ -I ./libs/irrKlang-64bit-1.6.0/include
OBJ = $(patsubst %.cpp,$(PATHOBJ)/%.o,$(SRC))

LINK_GLEW="https://downloads.sourceforge.net/project/glew/glew/snapshots/glew-20200115.tgz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fglew%2Ffiles%2Fglew%2Fsnapshots%2Fglew-20200115.tgz%2Fdownload&ts=1601925781"
LINK_FREEGLUT="https://downloads.sourceforge.net/project/freeglut/freeglut/2.8.1/freeglut-2.8.1.tar.gz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Ffreeglut%2Ffiles%2Ffreeglut%2F2.8.1%2Ffreeglut-2.8.1.tar.gz%2Fdownload%3Fuse_mirror%3Dfreefr%26download%3D&ts=1603729566"
LINK_GLM=https://github.com/g-truc/glm/archive/0.9.9.8.tar.gz
LINK_FREETYPE=https://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz
LINK_IRRKLANG=https://www.ambiera.at/downloads/irrKlang-64bit-1.6.0.zip

# Compil windows mingw64 : https://stackoverflow.com/questions/12886609/setting-up-glfw-with-mingw
FLGGRAPH_WINDOWS = -lglfw3dll -lopengl32
# Ubuntu compil (WSL OK, but you must install glfw 3.3.2 manually): https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project + https://stackoverflow.com/questions/40561937/using-powf-function-in-rtems-undefined-reference-to-powf
FLGGRAPH_UBUNTU = -lm -L./libs -lglut -lGL -lGLEW -lfreetype -std=c++11
# MacOs compil : ???
FLGGRAPH_MACOS = -lglfw3 -framework AppKit -framework OpenGL -framework IOKit -framework CoreVideo

ifeq ($(OS),Windows_NT)
    OS_COMPIL_CMD = $(OBJ) -o $(NAME) $(FLGGRAPH_WINDOWS); ResourceHacker -open $(NAME).exe -save $(NAME).exe -action addskip -res $() -mask ICONGROUP,MAIN,
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		THREADS = $(nproc)
		LINKER = ./libs/irrKlang-64bit-1.6.0/bin/linux-gcc-64/libIrrKlang.so
		AUDIO_PLUGIN_PATH = ./libs/irrKlang-64bit-1.6.0/bin/linux-gcc-64/ikpMP3.so
		AUDIO_PLUGIN = ikpMP3.so
    	OS_COMPIL_CMD = $(OBJ) $(LINKER) $(HEAD) -o $(NAME) $(FLGGRAPH_UBUNTU)
    endif
    ifeq ($(UNAME_S),Darwin)
		THREADS = $(shell sysctl -n hw.ncpu)
		LINKER = ./libs/irrKlang-64bit-1.6.0/bin/macosx-gcc/libirrklang.dylib
		AUDIO_PLUGIN_PATH = ./libs/irrKlang-64bit-1.6.0/bin/macosx-gcc/ikpMP3.dylib
		AUDIO_PLUGIN = ikpMP3.dylib
    	OS_COMPIL_CMD = $(OBJ) $(LINKER) $(HEAD) -o $(NAME) $(FLGGRAPH_MACOS)
    endif
endif

ifeq ($(shell uname),Darwin)
    ECHO    = echo
else
	ECHO    = echo
endif


TRUE = 1

FALSE = 0

COMPIL_CMD = g++

all: $(NAME)

$(PATHOBJ):
	@mkdir -p obj/Classes

gdb: fclean
	@make GDB_B=1

$(NAME): $(DEP) $(addprefix $(PATHSRC)/,$(SRC)) $(PATHOBJ) $(OBJ)
	@$(ECHO) "\r$(GREEN) The .o from $(NAME) are compiled.$(DEFAULT)"
ifeq ($(GDB_B),$(TRUE))
	$(GCCFGDB) $(OS_COMPIL_CMD)
else
	$(GCCF) $(OS_COMPIL_CMD)
endif
	@cp -rf $(AUDIO_PLUGIN_PATH) ./
	@$(ECHO) "$(GREEN)$(NAME)$(DEFAULT) created."

$(PATHOBJ)/%.o: $(PATHSRC)/%.cpp
	@$(ECHO) "\r$(GREEN) Compiling $@                      \c\033[K"
ifeq ($(GDB_B),$(TRUE))
	$(GCCFGDB) -c $< -o $@ $(HEAD)
else
	$(GCCF) -c $< -o $@ $(HEAD)
endif

clean:
	@$(ECHO) "All $(RED).o$(DEFAULT) are now deleted for $(NAME)."
	@rm -rf $(PATHOBJ)
	@rm -rf $()

fclean: clean
	@$(ECHO) "$(RED)$(NAME)$(DEFAULT) is now deleted."
	@rm -rf $(NAME)
	@rm -rf ./$(AUDIO_PLUGIN)

install_libs: clean_libs create_libs_folder install_glew install_freeglut3 install_glm install_freetype install_irrklang

create_libs_folder:
	mkdir -p libs_srcs
	mkdir -p libs

install_glew:
	wget -O libs_srcs/glew.tgz $(LINK_GLEW)
	tar -xvf libs_srcs/glew.tgz -C libs_srcs/
	make -j$(THREADS) -C libs_srcs/glew-2.2.0/
	cp -rf ./libs_srcs/glew-2.2.0/lib/* ./libs/
	cp -rf ./libs_srcs/glew-2.2.0/include/* ./includes/

install_freeglut3:
	wget -O libs_srcs/freeglut3.tgz $(LINK_FREEGLUT)
	tar -xvf libs_srcs/freeglut3.tgz -C libs_srcs/
	cd libs_srcs/freeglut-2.8.1/; mkdir tmp; ./configure --prefix=$(shell pwd)/libs_srcs/freeglut-2.8.1/tmp;
	make -j$(THREADS) -C libs_srcs/freeglut-2.8.1 all
	make -j$(THREADS) -C libs_srcs/freeglut-2.8.1 install
	cp -rf libs_srcs/freeglut-2.8.1/tmp/include/GL ./includes/
	cp -rf libs_srcs/freeglut-2.8.1/tmp/lib/* ./libs/

install_glm:
	wget -O libs_srcs/glm.tgz $(LINK_GLM)
	tar -xvf libs_srcs/glm.tgz -C libs_srcs/
	cmake -S libs_srcs/glm-0.9.9.8/ -B libs_srcs/glm-0.9.9.8/
	make -j$(THREADS) -C libs_srcs/glm-0.9.9.8/
	cp -rf ./libs_srcs/glm-0.9.9.8/glm ./includes/

install_freetype:
	wget -O libs_srcs/freetype2.tgz $(LINK_FREETYPE)
	tar -xvf libs_srcs/freetype2.tgz -C libs_srcs
	mkdir -p libs_srcs/freetype-2.10.0/builder
	cmake -S libs_srcs/freetype-2.10.0/ -B libs_srcs/freetype-2.10.0/builder
	make -j$(THREADS) -C libs_srcs/freetype-2.10.0/builder/
	mkdir -p ./includes/freetype2
	cp -rf libs_srcs/freetype-2.10.0/include/* ./includes/freetype2

install_irrklang:
	wget -O libs_srcs/irrklang.zip $(LINK_IRRKLANG)
	unzip libs_srcs/irrklang.zip -d libs_srcs/
	cp -rf libs_srcs/irrKlang-64bit-1.6.0 ./libs/

sudo_deps_linux:
	@sudo apt-get install build-essential libxmu-dev libxi-dev libgl-dev

clean_libs:
	rm -rf ./libs ./libs_srcs ./includes/freetype2 ./includes/GL ./includes/glm

re: fclean all

fast: fclean
	@make -j$(THREADS)

.PHONY: all clean fclean re set_gdb gdb install_libs