#ifndef CHUNK_HPP
#define CHUNK_HPP

// #include "vox.hpp"
// #include "Engine.hpp"
// #include "World.hpp"
class Noiser;
class Chunk
{
public:
	Chunk(void);
	Chunk(int x, int y, int z);
	~Chunk(void);

	// Noise chunk
	void noise(Noiser *noiser);
	bool isblocked(int x1, int y1, int z1, int x2, int y2, int z2);
	
	// Update vertex
	void update(bool doBind);

	// Render
	void render(void);

	// copy
	void copy_blk(uint8_t ptr[CX][CY][CZ]);
	void copy_chunk(Chunk *ptr);

	// Clear
	void clear_blk(void);

	// Getter
	uint8_t get(int x, int y, int z);

	// Setter
	void set(int x, int y, int z, uint8_t type);

	// Debug
	void print_chunk(void);

	// VARS //

	// Render Pos
	int ax, ay, az;
	// Seed pos
	int sx, sy, sz;

	// Neighbours ptr
	Chunk *left, *right, *below, *above, *front, *back;

	// GL States
	GLuint vbo;
	int elements;

	// States
	bool renoised;
	bool changed;
	bool noised;
	bool initialized;
	bool enable;
	bool v_alloc;

	uint8_t *v;

private:
	// Blocks
	uint8_t blk[CX][CY][CZ];
};

#endif