#version 430 core

struct Particle
{
	vec4 startPosition;
	float emitTime;
};

out vec2 texCoords;
out vec4 position; // maybe this is vec2
out vec4 color;

uniform mat4 view;
uniform mat4 projection;
uniform float currentTime;

layout (std430) buffer particleData
{
	Particle particles[];
};


void main()
{
	// Get id info
	uint particleID = gl_VertexID / 4; // Every group of 4 verts are ONE particle!
	uint cornerID = gl_VertexID % 4; // 0,1,2,3 = the corner of the particle "quad"

	// Grab one particle and its starting position
	Particle p = particles[particleID];
	vec3 pos = p.startPosition.xyz;

	// Calculate the age
	float age = currentTime - p.emitTime;

	// Perform an incredibly simple particle simulation:

	// Move right a little bit based on the particle's age
	pos += vec3(sin(currentTime), 0, 0) * age;

	// change scale
	float scale = (sin(currentTime) + 2) * .4;

	// change color
	color = vec4(sin(currentTime), 1, 1, 1);

	// change rotation


	// === Here is where you could do LOTS of other particle
	// === simulation updates, like rotation, acceleration, forces,
	// === fading, color interpolation, size changes, etc.

	// Offsets for the 4 corners of a quad - we'll only
	// use one for each vertex, but which one depends
	// on the cornerID above.
	vec2 offsets[4];
	offsets[0] = vec2(-1.0, +1.0);  // TL
	offsets[1] = vec2(+1.0, +1.0);  // TR
	offsets[2] = vec2(+1.0, -1.0);  // BR
	offsets[3] = vec2(-1.0, -1.0);  // BL


	// Billboarding
	pos += vec3(view[0][0], view[1][0], view[2][0]) * offsets[cornerID].x * scale; // RIGHT
	pos += vec3(view[0][1], view[1][1], view[2][1]) * offsets[cornerID].y * scale; // UP

	//pos *= scale;

	// Calculate output position
	//mat4 viewProj = projection * view;
	position = projection * view * vec4(pos, 1.0);

	gl_Position = position;

	// UVs for the 4 corners of a quad - again, only
	// using one for each vertex, but which one depends
	// on the cornerID above.
	vec2 uvs[4];
	uvs[0] = vec2(0, 0); // TL
	uvs[1] = vec2(1, 0); // TR
	uvs[2] = vec2(1, 1); // BR
	uvs[3] = vec2(0, 1); // BL
	texCoords = uvs[cornerID];
}