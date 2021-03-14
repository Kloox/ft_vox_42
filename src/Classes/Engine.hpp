#ifndef ENGINE_HPP
#define ENGINE_HPP

/*
** deps
*/

// #include "vox.hpp"

/*
** class
*/
class World;
class Chunk;
class Noiser;

struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

struct engine_state {
    const int transparent[16] = {2, 0, 0, 0, 1, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0};
    const char *blocknames[16] = {"air", "dirt", "topsoil", "grass", "leaves", "wood", "stone", "sand", "water", "glass", "brick", "coal", "woodrings", "gold", "diamond", "bedrock"};
    const float cutoff[16] = {
        0.5 / 16, 15.5 / 16, 4.5 / 16, 11.5 / 16,
        2.5 / 16, 13.5 / 16, 6.5 / 16, 9.5 / 16,
        1.5 / 16, 14.5 / 16, 5.5 / 16, 10.5 / 16,
        3.5 / 16, 12.5 / 16, 7.5 / 16, 8.5 / 16};
    char shader_version[14] = "#version 460\n";
    float volume;
    int i_display;
    bool anchor_cam;
    bool loading;
    int threads;
    int precompute_timer;
    int timebase;
    bool doSwap;
    int debug_show;
    bool worker;
    bool verbose;
    bool focus_on_transparent;
    int speedmult;
    bool collision;
    bool warp;
    bool exiting;
};

struct engine_ogl_state {
    GLuint program;
    GLint attribute_coord;
    GLint attribute_sky_coord;
    GLuint t_vbo;
    GLuint t_vao;
    GLuint sky_vao;
    GLuint sky_vbo;
    GLint uniform_mvp;
    GLuint cursor_vbo;
    GLint uniform_texture;
    GLint uniform_alpha;
    GLuint texture;
    GLuint skybox_cubemap;
};

struct engine_graphics_state {
    int window;
    int ww;
    int wh;
    bool aa;
    bool vsync;
    bool transparency;
    float fov;
    int frame;
    bool fullscreen;
    int framerate;
    int maxi;
    int renderDistance;
    int target_fps;
    int fpsToShow;
};

class Engine
{
public:
    Engine(void);
    ~Engine(void);

    // Init/Loading funcs
    int init_glew(void);
    int init_resources(void);
    void init_freetype_chars(void);
    void load_ressources(void);
    void load_world_texture(void);
    unsigned int load_skybox(void);

    // Movements
    void apply_collision(glm::vec3 lastP);
    void move(float movespeed);

    // Worlds mgt funcs
    void set_limit_by_time(int time);
    void set_chunk_index_by_pos(glm::vec3 pos);
    bool must_recompute_world(void);
    void noise_and_update_all(World *world, bool doBind);
    void swap_worlds(void);
    void apply_player_edits(void);

    // Rendering Funcs
    void draw_scene(glm::mat4 &mvp, glm::mat4 &view, glm::mat4 &projection, bool draw);
    void draw_loading_screen(std::string str, bool clear);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
    bool display_frame(void);
    void draw_hud(glm::mat4 &mvp, bool draw);
    void draw_debug(bool draw);
    void render_skybox(glm::mat4 &view, glm::mat4 &projection);

    // Debug & Utils funcs
    debug_value update_debug_values(bool draw, int init);
    int get_framerate(void);
    glm::vec3 get_chunk_position(glm::vec3 position);
    void write_keys(void);
    void write_usage(void);
    void print_false_enable(void);
    void set_debug_world_size(void);

    // VARS ///////

    //World size
    int	 scx;
    int	 scy;
    int	 scz;

    float scxf;
    float scyf;
    float sczf;

    // Worlds
    World *world;
    World *next_world;
    Noiser *noiser;


    // Libs class
    irrklang::ISoundEngine *sound;
    std::map<char, Character> Characters;
    FT_Library freetypeLib;
    FT_Face face;

    engine_ogl_state ogl_state;
    engine_state state;

    // debug
    int elements_world_one;
    int elements_world_two;

    // Hook state
    bool shift;
    unsigned int keys;
    bool autowalk;
    bool infinite;
    float mousespeed;
    float focus;

    // Precompute state
    bool computing;
    int *chunk_index;
    int *new_center;
    int barrier;
    player_edit edit_to_apply[512];
    int edit_size = 0;

    engine_graphics_state g_state;

private:
};

#endif