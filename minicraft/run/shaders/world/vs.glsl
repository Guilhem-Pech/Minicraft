#version 400

uniform float elapsed;
uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 nmat;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

//Variables en sortie
out vec3 normal;
out vec4 color;
out vec2 uv;
out vec3 wpos;
flat out int type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0

float noiseWater(vec4 v){
	
	vec2 dir = vec2(v.x, v.y);
	float l = dot(vec2(1,1),dir);
	float n = (sin(l/3 + elapsed) + 1)/2;
	
	l = dot(vec2(1.5,0.5),dir);
	n += (sin(l/3 + elapsed * 1.3) + 1)/4;

	l = dot(vec2(1.7, 0.3),dir);
	n += (sin(l/3 + elapsed * 1.6) + 1)/8;

	return n/1.75; 
}

void main()
{
	vec4 vecIn = vec4(vs_position_in,1.0); // Coord object
	vec4 vecInW = m * vecIn; // Coord world
	wpos = vecInW.xyz;
	// Vagues
	if(CUBE_EAU == vs_type_in){
		vecInW.z +=  noiseWater(vecInW) - 1 ;
	}


	vec4 vecInViewSpace = v * vecInW; // Coord view	
	vecInViewSpace.y -= length(vecInViewSpace.xyz)/4; // Effet planête

	gl_Position = p * vecInViewSpace;
		
	normal = (nmat * vec4(vs_normal_in,1.0)).xyz; 

	uv = vs_uv_in;

	//Couleur par défaut violet
	color = vec4(1.0,1.0,0.0,1.0);

	//Couleur fonction du type //TODO Mettre dans un tableau
	if(vs_type_in == CUBE_HERBE)
		color = vec4(0,1,0,1);
	if(vs_type_in == CUBE_TERRE)
		color = vec4(0.2,0.1,0,1);
	if(vs_type_in == CUBE_EAU)
		color = vec4(0.0,0.0,1.0,0.7);	
}