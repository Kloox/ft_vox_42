#ifndef __NOISER_H__
#define __NOISER_H__

#include "vox.hpp"
// 2D SIMPLEX NOISE
#define NOISE_2D_MULTIPLIER 10 // Default 4
#define NOISE_2D_DIVIDER 2048.0 // Default 256.0
#define NOISE_2D_PERSISTENCE 0.8 // Default 0.8
#define NOISE_2D_OCTAVES 7 // Default 5
#define NOISE_2D_STRENGTH 1.0 // Default 1.0
#define NOISE_2D_SCALE 1.0 // Default 1.0

// 3D SIMPLEX NOISE
#define NOISE_3D_SIMPLEX_DIVIDER 16.0 // Default 16.0
#define NOISE_3D_SIMPLEX_PERSISTENCE 0.1f // default 0.1f
#define NOISE_3D_SIMPLEX_OCTAVES 2 // Default 2
#define NOISE_3D_SIMPLEX_STRENGTH 1.0 // Default 1.0
#define NOISE_3D_SIMPLEX_SCALE 1.0 // Default 1.0

// 3D PERLIN NOISE
#define NOISE_3D_PERLIN_DIVIDER 16.0
#define NOISE_3D_PERLIN_PERSISTENCE 0.1f
#define NOISE_3D_PERLIN_OCTAVES 2
#define NOISE_3D_PERLIN_STRENGTH 1.0
#define NOISE_3D_PERLIN_SCALE 1.0

// 3D CELLULAR NOISE
#define NOISE_3D_CELL_DIVIDER 16.0 // Default 16.0
#define NOISE_3D_CELL_FREQUENCY 0.4f // Default 0.40f
#define NOISE_3D_CELL_JITTER 0.45f // Default 0.45f
#define NOISE_3D_CELL_MAP_LOW 0 // Default 0
#define NOISE_3D_CELL_MAP_MAX 3 // Default 3


class Noiser
{
    public:
        Noiser(void);
        ~Noiser(void);

        int seed;

        int noise_2d_multiplier;
        float noise_2d_divider;
        float noise_2d_persistence;
        int   noise_2d_octaves;
        float noise_2d_strength;
        float noise_2d_scale;

        int noise_3d_simplex_divider;
        float noise_3d_simplex_persistence;
        float noise_3d_simplex_octaves;
        float noise_3d_simplex_strength;
        float noise_3d_simplex_scale;

        int noise_3d_perlin_divider;
        float noise_3d_perlin_persistence;
        float noise_3d_perlin_octaves;
        float noise_3d_perlin_strength;
        float noise_3d_perlin_scale;

        float noise_3d_cell_divider;
        float noise_3d_cell_frequency;
        float noise_3d_cell_jitter;
        int noise_3d_cell_map_low;
        int noise_3d_cell_map_max;

        float SQRT3;
        float F2;
        float G2;
        float G3;
        float F3;

	    float noise2d(float x, float y, int octaves, float persistence);
	    float noise3d_abs(float x, float y, float z, int octaves, float persistence);
        float noise_simplex(unsigned char offset, float x, float y);
        float noise_simplex(unsigned char offset, float x, float y, float z);
        float noise_perlin(unsigned char offset, float x, float y, float z);
        
        float gradCoord2D(unsigned char offset, int x, int y, float xd, float yd);
        float lerp(float a, float b, float c);
        float gradCoord3D(unsigned char offset, int x, int y, int z, float xd, float yd, float zd);

        unsigned char Index2D_12(unsigned char offset, int x, int y);
        unsigned char Index3D_12(unsigned char offset, int x, int y, int z);

	    unsigned char Index3D_256(unsigned char offset, int x, int y, int z);
	    float cellular_noise(float x, float y, float z, float frequency);
	    float cellular(float x, float y, float z, int cellDist0, int cellDist1);
	    float noise_3d_perlin(float x, float y, float z, int octaves, float persistence);
        void set_seed(int seed);
        void create_seed(void);

        unsigned char m_perm[512];
        unsigned char m_perm12[512];
    private:
    protected:
};

#endif