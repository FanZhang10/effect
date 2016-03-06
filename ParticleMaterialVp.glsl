#version 400

// Attributes passed automatically by OGRE
in vec3 vertex;
in vec3 normal;
in vec4 colour;

// Attributes passed with the material file
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform vec3 up_vec;
uniform float timer;
uniform vec3 object_colour;

// Attributes forwarded to the geometry shader
out vec3 vertex_colour;

out vec4 particle_colour;
out float particle_id;

void main()
{
    // Let time cycle every four seconds
	float circtime = timer - 4.0 * floor(timer / 4);
	float t = circtime; // Our time parameter
	
	// Define particle id
	particle_id = colour.r;

	// Settings for the explosion
	// Could also define these in the material file to have multiple particle systems with different settings
    float grav = 0.5; // Gravity
    float slow = 0.1; // Allows to slow down the explosion, control the speed of the explosion
	
	//float accel = 1.2; // An acceleration applied to the particles coming from some attraction force

	// Let's first work in model space (apply only world matrix)
	//vec3 position = vertex;
	//position -= slow*up_vec*accel*t*t; // Particle moves down
	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 1.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += norm.x*t*slow - grav*slow*up_vec.x*t*t;
    position.y += norm.y*t*slow - grav*slow*up_vec.y*t*t;
    position.z += norm.z*t*slow - grav*slow*up_vec.z*t*t;
	//x(t) = x(0) + v*t + 0.5*g*t*t.
	//position.x += norm.x + t*slow + 0.5*grav*up_vec.x*t*t;
    //position.y += norm.y + t*slow + 0.5*grav*up_vec.y*t*t;
    //position.z += norm.z + t*slow + 0.5*grav*up_vec.z*t*t;

	// Now apply view transformation
	gl_Position = view_mat * position;
	//gl_Position = view_mat * world_mat * vec4(position, 2.0); 
	  
	// Define color of vertex
	//vertex_colour = colour.rgb; // Color define during the construction of the particles
	vertex_colour = object_colour; // Uniform color assigned in material file

	// Define amount of blending depending on the cyclic time    
	//float alpha = 1.0 - circtime*circtime;
	//particle_colour = vec4(1.0, 1.0, 1.0, alpha);
}
