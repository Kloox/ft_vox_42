
#ifndef VOX_HPP
# define VOX_HPP

/*
** deps
*/

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <fstream>
#include <thread>
#include <map>
#include <mutex>
#include <random>
#include <iostream>




#include "GL/glew.h"
#include "GL/freeglut.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/noise.hpp"

#include "freetype2/ft2build.h"
#include FT_FREETYPE_H

#include "irrKlang.h"
/*
** defines
*/

#define FONT_PATH "./font/font.ttf"

# define WIDTH_MIN 320
# define HEIGHT_MIN 240
# define WIDTH_MAX 15360
# define HEIGHT_MAX 8460

# define WORLD_MIN 8
# define WORLD_MAX 128

// Resolution
#define RES_X 1920
#define RES_Y 1080

// Size of one chunk in blocks
#define CX 16
#define CY 32
#define CZ 16

#define CXF 16.0f
#define CYF 32.0f
#define CZF 16.0f

// Number of chunks in the world
// #define SCX 32
// #define SCY 8
// #define SCZ 32

// #define SCXF 32.0f
// #define SCYF 8.0f
// #define SCZF 32.0f

// Sea level
#define SEALEVEL 4

// Number of VBO slots for chunks
//#define CHUNKSLOTS ((SCX * SCY * SCZ))

// Range raycast
#define RAYCAST_LENGTH 500
#define SMOOTH_BIND_STEPS_CUT 1

#define SEED 1542632142

#define PLAYER_SPEED 1
#define PLAYER_SPRINT_SPEED_MULT 20.0f

// AUDIO
#define AUDIO_AMBIENCE_PATH "audio/ambience.mp3"
#define AUDIO_BLOCK_PATH "audio/block.wav"

#define VERTICE_SIZE (CX*CY*CZ*18)

/*
** structs
*/

typedef struct byte4 {
	public:
	uint8_t x, y, z, w;
	byte4() {}
	byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w): x(x), y(y), z(z), w(w) {}
} byte4;

typedef struct player_edit {
	int	x,y,z;
	uint8_t blk;
	player_edit() {}
	player_edit(int x, int y, int z, uint8_t blk): x(x), y(y), z(z), blk(blk) {}
} player_edit;

typedef struct to_update {
	int x,y,z;
	to_update() {}
	to_update(int x, int y, int z): x(x), y(y), z(z) {};
} to_update;

typedef struct vec3int {
	int x,y,z;
	vec3int() {}
	vec3int(int x, int y, int z): x(x), y(y), z(z) {};
} vec3int;

typedef struct debug_value {
	std::string fps;
	std::string wp;
	std::string wpp;
	std::string wcp;
	std::string wcpp;
	std::string lp;
	std::string lpp;
	std::string lcp;
	std::string lcpp;
	std::string forward;
	std::string right;
	std::string up;
	std::string lookat;
	std::string angle;
	std::string facing;
	std::string velocity;
	std::string infinite;
	std::string barrierl;
	std::string lpt;
	std::string cxn;
	std::string rd;
	std::string ft;
	std::string sampling;
	std::string aa;
	std::string vs;
	glm::vec3 color;
} debug_value;

/*
** Classes
*/

#include "Chunk.hpp"
#include "World.hpp"
#include "Engine.hpp"
#include "Noiser.hpp"

/*
** Textures
*/

#include "../src/textures.cpp"
#include "../skybox/back.cpp"
#include "../skybox/bottom.cpp"
#include "../skybox/front.cpp"
#include "../skybox/left.cpp"
#include "../skybox/right.cpp"
#include "../skybox/top.cpp"


/*
** callbacks.cpp
*/

// static void reshape(int w, int h);
// static void idle(void);
// static void specialup(int key, int x, int y);
// static void special(int key, int x, int y);
// static void keyboard(unsigned char key, int x, int y);
// static void keyboardup(unsigned char key, int x, int y);
// static void display(void);
// static void motion(int x, int y);
// static void mouse(int button, int state, int x, int y);

/*
** utils.cpp
*/

char* file_read(const char* filename);
GLuint create_shader(const char* filename, GLenum type);
GLuint create_program(const char* vertexfile, const char *fragmentfile);
GLint get_attrib(GLuint program, const char *name);
GLint get_uniform(GLuint program, const char *name);
void print_log(GLuint object);
void print_process_mem_usage(std::string prefix);
const char * BoolToString(bool b);
int get_mem_usage(void);
void do_exit(Engine *eng);

#endif