#ifndef WORLD_HPP
# define WORLD_HPP

// #include "vox.hpp"
// #include "Chunk.hpp"
#define CUT_FORMULA ((x + dx > engine->scx - 1 || x + dx < 0) || (z + dz > engine->scz - 1 || z + dz < 0))
// #include "Engine.hpp"

class Chunk;

class World {
    public:

        // Class funcs
        World(void);
        ~World(void);
        void copy_world(World * ptr);
        void render(const glm::mat4 &pv, int renderDistance, GLint uniform_mvp);

        // Vbo alloc
        void create_all_buffers(void);

        // Camera funcs
        void adjust_camera(World *src);
        void update_vectors(void);

        // Precompute funcs
        void precompute(World* src, int *new_center);
        void precompute_noise(int dx, int dz, int split);
        void precompute_swap_chunks(int dx, int dz, Chunk *save[128][8][128]);
        void precompute_noise_and_update(int dx, int dz);

        // Noise funcs
        void noise_all(void);
        void noise_all_multithread(int split);
        void noise_tab_multithread(std::vector<vec3int> noise_arr, int split);
        void noise_tab_range(std::vector<vec3int> noise_arr, int start, long unsigned int end);
        void noise_worker_range(int xstart, int xend, int ystart, int yend, int zstart, int zend);

        // Update funcs
        void update_all(bool doBind);
        void update_all_multithread(bool doBind, int split);
        void update_worker_range(int xstart, int xend, int ystart, int yend, int zstart, int zend, bool doBind);

        // Bind funcs
        void bind_all_buffers(void);

        // Getter
        Chunk *getChunkPtr(int x, int y, int z);
        uint8_t getChunk(int x, int y, int z);

        // Setter state funcs
        void assign_chunk_ptrs(void);
        void set_all_to_disabled(void);
        void set_all_to_not_changed(void);
        void set_all_to_not_noised(void);
        void set_all_cleared_blk(void);
        void setChunk(int x, int y, int z, uint8_t type);
        
        // VARS

        // Noiser
        Noiser *noiser_ptr;

        // Position & Camera
        glm::vec3 position;
        // glm::vec3 lastPosition;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 lookat;
        glm::vec3 angle;

        // States
        int mx, my, mz;
        int cam_dx, cam_dz;
        int shiftX, shiftY, shiftZ;
        int face;
        uint8_t buildtype;
        Chunk   *c[128][8][128];
    private:
};

#endif