#include "vox.hpp"

extern Engine *engine;


static int tt = glutGet(GLUT_ELAPSED_TIME);

void Chunk::render(void)
{
	if (this->changed)
		this->update(true);

	if (!(this->elements))
		return;

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glVertexAttribPointer(engine->ogl_state.attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, this->elements);
}

Chunk::~Chunk(void)
{
	delete [] this->v;
	return ;
}

Chunk::Chunk()
{
	memset(this->blk, 0, sizeof this->blk);
	this->ax = this->ay = this->az = this->sx = this->sy = this->sz = 0;
	this->left = this->right = this->below = this->above = this->front = this->back = 0;
	this->changed = true;
	this->initialized = false;
	this->renoised = false;
	this->noised = false;
	this->enable = true;
	this->elements = 0;
	this->vbo = 0;
	this->v_alloc = false;
}

Chunk::Chunk(int x, int y, int z) : ax(x), ay(y), az(z), sx(x), sy(y), sz(z)
{
	memset(this->blk, 0, sizeof this->blk);
	this->left = this->right = this->below = this->above = this->front = this->back = 0;
	this->changed = true;
	this->initialized = false;
	this->renoised = false;
	this->noised = false;
	this->enable = true;
	this->elements = 0;
	this->vbo = 0;
	this->v_alloc = false;
}

void Chunk::print_chunk(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Chunk addr: %p\n", this);
	fprintf(stderr, "Chunk APOS: (%d %d %d)\n", this->ax, this->ay, this->az);
	fprintf(stderr, "Chunk SPOS: (%d %d %d)\n", this->sx, this->sy, this->sz);
	fprintf(stderr, "Chunk state: Changed : %s\tNoised : %s\tInitialized : %s\n", BoolToString(this->changed), BoolToString(this->noised), BoolToString(this->initialized));
	fprintf(stderr, "Chunk G state : VBO N° : %d\telements: %d\tenable : %s\n", this->vbo, this->elements, BoolToString(this->enable));
	fprintf(stderr, "Chunk Renoised : %s\n", BoolToString(this->renoised));
	fprintf(stderr, "\n");
}

void Chunk::clear_blk(void)
{
	memset(this->blk, 0, sizeof(uint8_t) * CX * CY * CZ);
	this->elements = 0;
}

void Chunk::copy_blk(uint8_t ptr[CX][CY][CZ])
{
	this->clear_blk();
	for (int x = 0; x < CX; x++)
		for (int y = 0; y < CY; y++)
			for (int z = 0; z < CZ; z++)
			{
				this->blk[x][y][z] = ptr[x][y][z];
			}
}

void Chunk::copy_chunk(Chunk *ptr)
{
	this->ax = ptr->ax;
	this->ay = ptr->ay;
	this->az = ptr->az;
	this->sx = ptr->sx;
	this->sy = ptr->sy;
	this->sz = ptr->sz;
	this->changed = ptr->changed;
	this->noised = ptr->noised;
	this->initialized = ptr->initialized;
	this->enable = ptr->enable;
	this->copy_blk(ptr->blk);
}

uint8_t Chunk::get(int x, int y, int z)
{
	if (x < 0)
		return this->left ? this->left->blk[x + CX][y][z] : 0;
	if (x >= CX)
		return this->right ? this->right->blk[x - CX][y][z] : 0;
	if (y < 0)
		return this->below ? this->below->blk[x][y + CY][z] : 0;
	if (y >= CY)
		return this->above ? this->above->blk[x][y - CY][z] : 0;
	if (z < 0)
		return this->front ? this->front->blk[x][y][z + CZ] : 0;
	if (z >= CZ)
		return this->back ? this->back->blk[x][y][z - CZ] : 0;
	return this->blk[x][y][z];
}

bool Chunk::isblocked(int x1, int y1, int z1, int x2, int y2, int z2)
{
	// Invisible blocks are always "blocked"
	if (!(this->blk[x1][y1][z1]))
		return true;

	// Leaves do not block any other block, including themselves
	if (engine->state.transparent[this->get(x2, y2, z2)] == 1)
		return false;

	// Non-transparent blocks always block line of sight
	if (!(engine->state.transparent[get(x2, y2, z2)]))
		return true;

	// Otherwise, LOS is only blocked by blocks if the same transparency type
	return engine->state.transparent[this->get(x2, y2, z2)] == engine->state.transparent[this->blk[x1][y1][z1]];
}

void Chunk::noise(Noiser *noiser)
{
	if (this->noised)
		return;
	else
		this->noised = true;

	for (int x = 0; x < CX; x++)
	{
		for (int z = 0; z < CZ; z++)
		{
			// Land height
			float n = noiser->noise2d((x + sx * CX) / noiser->noise_2d_divider, (z + sz * CZ) / noiser->noise_2d_divider, noiser->noise_2d_octaves, noiser->noise_2d_persistence) * noiser->noise_2d_multiplier;

			int h = n * 2;

			// Land blocks
			for (int y = 0; y < CY; y++)
			{
				if (this->ay == -4 && y == 0) {
					this->blk[x][y][z] = 15;
					continue;
				}
				// Are we above "ground" level?
				bool ground = y + sy * CY >= h;
				bool sea = y + sy * CY < SEALEVEL;
				unsigned char rand_seed_char = noiser->m_perm[(x & 0xff) + noiser->m_perm[(y & 0xff) + noiser->m_perm[(z & 0xff)]]];
				if (ground)
				{
					// If we are not yet up to sea level, fill with water blocks
					if (sea)
					{
						this->blk[x][y][z] = 8;
						continue;
						// Otherwise, we are in the air
					}
					else
					{
						// A tree!
						if (this->get(x, y - 1, z) == 3 && (rand_seed_char & 0xff) == 0)
						{
							// Trunk
							h = (noiser->m_perm12[x + y + z] & 0x1) + 4;
							for (int i = 0; i < h; i++)
								this->set(x, y + i, z, 5);

							// Leaves
							for (int ix = -3; ix <= 3; ix++)
							{
								for (int iy = -3; iy <= 3; iy++)
								{
									for (int iz = -3; iz <= 3; iz++)
									{
										if (ix * ix + iy * iy + iz * iz < 8 + (rand_seed_char & 1) && !(this->get(x + ix, y + h + iy, z + iz)))
											this->set(x + ix, y + h + iy, z + iz, 4);
									}
								}
							}
						}
						break;
					}
				}

				// Random value used to determine land type
				float c = noiser->cellular_noise((x + sx * CX) / noiser->noise_3d_cell_divider, (y + sy * CY) / noiser->noise_3d_cell_divider, (z + sz * CZ) / noiser->noise_3d_cell_divider, noiser->noise_3d_cell_frequency);
				float r = noiser->noise_3d_perlin((x + sx * CX) / noiser->noise_3d_perlin_divider, (y + sy * CY) / noiser->noise_3d_perlin_divider, (z + sz * CZ) / noiser->noise_3d_perlin_divider, noiser->noise_3d_perlin_octaves, noiser->noise_3d_perlin_persistence);
				
				// Sand layer
				if (n + r * 5 < 4)
					this->blk[x][y][z] = 7;
				// Dirt layer, but use grass blocks for the top
				else if (n + r * 5 < 12)
					this->blk[x][y][z] = (h < SEALEVEL || y + sy * CY < h - 1) ? 1 : 3;
				// Rock layer
				else if (r < 0.9355)
					this->blk[x][y][z] = 6;
				if (c > 0.70f && !(ground))
					this->blk[x][y][z] = 0;
				else if (c > 0.5f)
				{
					if (c > 0.69887f && this->blk[x][y][z] != 11 && this->blk[x][y][z] != 13 && this->blk[x][y][z] != 14) {
						if (rand_seed_char < 40 && (this->ay == (engine->scy / 2) * -1))
							this->blk[x][y][z] = 14;
						else if (rand_seed_char < 60 && (this->ay < ((engine->scy / 2) * -1) + 3))
							this->blk[x][y][z] = 13;
						else if (rand_seed_char < 200)
							this->blk[x][y][z] = 11;
						else
							this->blk[x][y][z] = 6;
					} else if (this->blk[x][y][z] != 11 && this->blk[x][y][z] != 13 && this->blk[x][y][z] != 14)
							this->blk[x][y][z] = 6;
				}
				else if (this->ay >= 0) {
					if (rand_seed_char < 100 && rand_seed_char > 90 && this->blk[x][y][z] == 6)
						this->blk[x][y][z] = 11;
				}
			}
		}
	}
	this->changed = true;
}

void Chunk::set(int x, int y, int z, uint8_t type)
{
	// If coordinates are outside this chunk, find the right one.
	if (x < 0)
	{
		if (this->left)
			this->left->set(x + CX, y, z, type);
		return;
	}
	if (x >= CX)
	{
		if (this->right)
			this->right->set(x - CX, y, z, type);
		return;
	}
	if (y < 0)
	{
		if (this->below)
			this->below->set(x, y + CY, z, type);
		return;
	}
	if (y >= CY)
	{
		if (this->above)
			this->above->set(x, y - CY, z, type);
		return;
	}
	if (z < 0)
	{
		if (this->front)
			this->front->set(x, y, z + CZ, type);
		return;
	}
	if (z >= CZ)
	{
		if (this->back)
			this->back->set(x, y, z - CZ, type);
		return;
	}

	// Change the block
	this->blk[x][y][z] = type;
	this->changed = true;

	// When updating blocks at the edge of this chunk,
	// visibility of blocks in the neighbouring chunk might change.
	if (x == 0 && this->left)
		this->left->changed = true;
	if (x == CX - 1 && this->right)
		this->right->changed = true;
	if (y == 0 && this->below)
		this->below->changed = true;
	if (y == CY - 1 && this->above)
		this->above->changed = true;
	if (z == 0 && this->front)
		this->front->changed = true;
	if (z == CZ - 1 && this->back)
		this->back->changed = true;
}

void Chunk::update(bool doBind)
{
	byte4 vertex[VERTICE_SIZE];
	int i = 0;
	int merged = 0;
	bool vis = false;

	// View from negative x

	for (int x = CX - 1; x >= 0; x--)
	{
		for (int y = 0; y < CY; y++)
		{
			for (int z = 0; z < CZ; z++)
			{
				// Line of sight blocked?
				if (isblocked(x, y, z, x - 1, y, z))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];
				uint8_t side = this->blk[x][y][z];

				// Grass block has dirt sides and bottom
				if (top == 3)
				{
					bottom = 1;
					side = 2;
					// Wood blocks have rings on top and bottom
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				// Same block as previous one? Extend it.
				if (vis && z != 0 && this->blk[x][y][z] == this->blk[x][y][z - 1])
				{
					vertex[i - 5] = byte4(x, y, z + 1, side);
					vertex[i - 2] = byte4(x, y, z + 1, side);
					vertex[i - 1] = byte4(x, y + 1, z + 1, side);
					merged++;
					// Otherwise, add a new quad.
				}
				else
				{
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
				}

				vis = true;
			}
		}
	}

	// View from positive x

	for (int x = 0; x < CX; x++)
	{
		for (int y = 0; y < CY; y++)
		{
			for (int z = 0; z < CZ; z++)
			{
				if (isblocked(x, y, z, x + 1, y, z))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];
				uint8_t side = this->blk[x][y][z];

				if (top == 3)
				{
					bottom = 1;
					side = 2;
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				if (vis && z != 0 && this->blk[x][y][z] == this->blk[x][y][z - 1])
				{
					vertex[i - 4] = byte4(x + 1, y, z + 1, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y, z + 1, side);
					merged++;
				}
				else
				{
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
				}
				vis = true;
			}
		}
	}

	// View from negative y

	for (int x = 0; x < CX; x++)
	{
		for (int y = CY - 1; y >= 0; y--)
		{
			for (int z = 0; z < CZ; z++)
			{
				if (isblocked(x, y, z, x, y - 1, z))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];

				if (top == 3)
				{
					bottom = 1;
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				if (vis && z != 0 && this->blk[x][y][z] == this->blk[x][y][z - 1])
				{
					vertex[i - 4] = byte4(x, y, z + 1, bottom + 128);
					vertex[i - 2] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i - 1] = byte4(x, y, z + 1, bottom + 128);
					merged++;
				}
				else
				{
					vertex[i++] = byte4(x, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
				}
				vis = true;
			}
		}
	}

	// View from positive y

	for (int x = 0; x < CX; x++)
	{
		for (int y = 0; y < CY; y++)
		{
			for (int z = 0; z < CZ; z++)
			{
				if (isblocked(x, y, z, x, y + 1, z))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];

				if (top == 3)
				{
					bottom = 1;
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				if (vis && z != 0 && this->blk[x][y][z] == this->blk[x][y][z - 1])
				{
					vertex[i - 5] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 2] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, top + 128);
					merged++;
				}
				else
				{
					vertex[i++] = byte4(x, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, top + 128);
				}
				vis = true;
			}
		}
	}

	// View from negative z

	for (int x = 0; x < CX; x++)
	{
		for (int z = CZ - 1; z >= 0; z--)
		{
			for (int y = 0; y < CY; y++)
			{
				if (isblocked(x, y, z, x, y, z - 1))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];
				uint8_t side = this->blk[x][y][z];

				if (top == 3)
				{
					bottom = 1;
					side = 2;
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				if (vis && y != 0 && this->blk[x][y][z] == this->blk[x][y - 1][z])
				{
					vertex[i - 5] = byte4(x, y + 1, z, side);
					vertex[i - 3] = byte4(x, y + 1, z, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z, side);
					merged++;
				}
				else
				{
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
				}
				vis = true;
			}
		}
	}

	// View from positive z

	for (int x = 0; x < CX; x++)
	{
		for (int z = 0; z < CZ; z++)
		{
			for (int y = 0; y < CY; y++)
			{
				if (isblocked(x, y, z, x, y, z + 1))
				{
					vis = false;
					continue;
				}

				uint8_t top = this->blk[x][y][z];
				uint8_t bottom = this->blk[x][y][z];
				uint8_t side = this->blk[x][y][z];

				if (top == 3)
				{
					bottom = 1;
					side = 2;
				}
				else if (top == 5)
				{
					top = bottom = 12;
				}

				if (vis && y != 0 && this->blk[x][y][z] == this->blk[x][y - 1][z])
				{
					vertex[i - 4] = byte4(x, y + 1, z + 1, side);
					vertex[i - 3] = byte4(x, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, side);
					merged++;
				}
				else
				{
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
				}
				vis = true;
			}
		}
	}

	this->changed = false;
	this->elements = i;

	if (this->v_alloc) 
		delete [] this->v;
	this->v = new uint8_t[this->elements * 4];
	this->v_alloc = true;
	memcpy(this->v, vertex, this->elements * sizeof(byte4));
	if (!(this->elements))
	{
		return;
	}

	// Upload vertices
	if (doBind)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, this->elements * sizeof(byte4), this->v, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}