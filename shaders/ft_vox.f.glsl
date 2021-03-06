in vec4 texcoord;
in vec2 text_coord;
in vec3 skybox_tex_coord;
uniform sampler2D texture_2d;
uniform float alpha;
uniform vec3 textColor;
uniform int text_render;
uniform samplerCube skybox;
out vec4 FragColor;

const vec4 fogcolor = vec4(0.6, 0.8, 1.0, 1.0);
const float fogdensity = .00003;

void main(void) {
	vec4 finalColor;
	if (text_render == 0) {
	vec2 coord2d;
	float intensity;

	// If the texture index is negative, it is a top or bottom face, otherwise a side face
	// Side faces are less bright than top faces, simulating a sun at noon
	if(texcoord.w < 0.0) {
		coord2d = vec2((fract(texcoord.x) + texcoord.w) / 16.0, texcoord.z);
		intensity = 1.0;
	} else {
		coord2d = vec2((fract(texcoord.x + texcoord.z) + texcoord.w) / 16.0, -texcoord.y);
		intensity = 0.85;
	}
	
	vec4 color = texture(texture_2d, coord2d);

	// Very cheap "transparency": don't draw pixels with a low alpha value
	if(color.a < alpha)
		discard;

	// Attenuate sides of blocks
	color.xyz *= intensity;

	// Calculate strength of fog
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog = clamp(exp(-fogdensity * z * z), 0.2, 1.0);

	// Final color is a mix of the actual color and the fog color
	// finalColor = mix(fogcolor, color, fog);
		FragColor = mix(fogcolor, color, fog);
	} else if (text_render == 1) {
    	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_2d, text_coord).r);
    	// // finalColor = vec4(textColor, 1.0) * sampled;		
		FragColor = vec4(textColor, 1.0) * sampled;
	} else {
		FragColor = texture(skybox, texcoord.xyz);
		// FragColor = texture(texture_2d, texcoord.xy);
		// FragColor = vec4(1.0,1.0,1.0, 1.0);
	}
}
