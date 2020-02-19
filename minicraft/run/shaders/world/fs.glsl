#version 400
uniform float elapsed;
//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
in vec3 wpos;
flat in int type;
out vec4 color_out;

uniform vec3 lightDir;
uniform vec3 camPos;
uniform vec3 skyColor;
uniform vec3 sunColor;
//Globales
const float ambientLevel = 0.4;

#define CUBE_EAU 4.0

float noiseWater(vec3 v){
	vec2 dir = vec2(v.x, v.y);
	float l = dot(vec2(1,1),dir);
	float n = (sin(l/3 + elapsed) + 1)/2;
	
	l = dot(vec2(1.5,0.5),dir);
	n += (sin(l/3 + elapsed * 1.3) + 1)/4;

	l = dot(vec2(1.7, 0.3),dir);
	n += (sin(l/3 + elapsed * 1.6) + 1)/8;

	return n/1.75; // -1
}


void main()
{

	vec3 N = normal;
	// WaterNormal
	if (type == CUBE_EAU){
			vec3 a = wpos;
			a.z += noiseWater(a) - 1;

			vec3 b = wpos + vec3(0.2,0,0);
			b.z += noiseWater(b) - 1;

			vec3 c = wpos + vec3(0,0.2,0);
			c.z += noiseWater(c) - 1;

			N = cross(normalize(b-a),normalize(c-a));
	}





	vec3 toLight = normalize(lightDir);
	vec3 vue = normalize(camPos - wpos);
	vec4 c = color;
		//Fresnel 
	float fresnel = max(0, dot(N,vue));
	fresnel = pow(fresnel,0.7);
	c.a = max(1 - fresnel, 0.2);


	

	// Diffuse
	float diff = max(0,dot(toLight, N)); 
	c.rgb *= diff;

	// Speculaire
	vec3 H = normalize(toLight + vue);
	float spec = dot(N, H);
	spec = pow(spec,300);
	c.rgb += sunColor * spec * diff;

	//ambient
	c.rgb += 0.1 * skyColor * (1-diff);

	color_out =  vec4(sqrt(c.rgb), c.a);
	//vec4(sqrt(color.xyz * max(0,dot(toLight,normal)) * 0.97 + 0.03 * vec3(0.8,0.9,1)),color.a);
}