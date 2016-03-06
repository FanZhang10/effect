#version 400

// Attributes passed from the geometry shader
in vec4 frag_colour;

in vec2 tex_coord;

// Attributes passed with the material file
uniform vec3 object_colour;
uniform sampler2D tex_samp;

void main (void)
{

	// Get pixel from texture
	vec4 outval = texture(tex_samp, tex_coord);

	// Very simple fragment shader, but we can do anything we want here
	// We could apply a texture to the particle, illumination, etc.
	outval = vec4(outval.r*object_colour.r, outval.g*object_colour.g, outval.b*object_colour.b, sqrt(sqrt(outval.r))*frag_colour.a);
	gl_FragColor = outval;
	//gl_FragColor = frag_colour;
}