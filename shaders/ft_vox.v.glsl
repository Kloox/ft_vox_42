layout (location = 0) in vec4 coord;
uniform mat4 mvp;
uniform mat4 sky_mvp;
uniform mat4 projection;
uniform int text_render;
out vec4 texcoord;
out vec3 skybox_tex_coord;
out vec2 text_coord;


void main(void) {
	if (text_render == 0) {
		// Just pass the original vertex coordinates to the fragment shader as texture coordinates
		texcoord = coord;

		// Apply the model-view-projection matrix to the xyz components of the vertex coordinates
		gl_Position = mvp * vec4(coord.xyz, 1);
	} else if (text_render == 1) {
		text_coord = coord.zw;
		gl_Position = projection * vec4(coord.xy, 0.0, 1.0);
	} else {
		texcoord = coord;
		vec4 pos = sky_mvp * vec4(coord.xyz, 1.0);
		gl_Position = pos;
	}
}
