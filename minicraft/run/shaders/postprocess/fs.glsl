#version 400

in vec2 uv;

uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;
uniform vec2 near_far;

out vec4 color_out;

float LinearizeDepth(float z)
{
	float n = near_far.x; // camera z near
  	float f = near_far.y; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( TexColor , uv );
	float depth = texture2D( TexDepth , uv ).r;	
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);

	

	// Blur
	vec3 colorBlur = vec3(0,0,0);
	float nbPixels = 0;
	float size = 2;
	for(float x=-1; x<size; ++x){
		for(float y= -size; y<= size; ++y) {
			++nbPixels;
			vec2 delta = vec2(x*xstep, y*ystep)*2;
			colorBlur += texture2D( TexColor , uv + delta ).rgb;
		}
	}
	colorBlur /= nbPixels;
	
	float depthCenter = texture2D( TexDepth , vec2(0.5,0.5)).r;		
	depthCenter = LinearizeDepth(depth);
	//color.rgb = mix(color.rgb, colorSum, pow(abs(depthCenter - depth),0.2)); //TODO Régler ça correctement, sortir le depthcenter ? 

	
	// Sobel
	vec3 colorSobel = vec3(0,0,0);
	float depthSobel = 0;
	nbPixels = 0;
	size = 1;
	for(float x=-1; x<size; ++x){
		for(float y= -size; y<= size; ++y) {
			++nbPixels;
			vec2 delta = vec2(x*xstep, y*ystep) * 1 ;
			colorSobel += texture2D( TexColor , uv + delta ).rgb; // Effet super cool
			depthSobel +=  LinearizeDepth(texture2D( TexDepth , uv + delta ).r);
		}
	}
	depthSobel -= nbPixels * depth;
	colorSobel -= nbPixels * color.rgb;
	
	color.rgb += abs(colorSobel);
	color.rgb += abs(vec3(depthSobel,0,0)); 
	

	// Abération Chromatique 
	color.r =  texture2D( TexColor , uv + vec2(xstep,0)*20).r; 


	// SSAO à la rache 
	float depthSSAO = 0;
	nbPixels = 0;
	size = 1;
	for(float x=-1; x<size; ++x){
		for(float y= -size; y<= size; ++y) {
			++nbPixels;
			vec2 delta = vec2(x*xstep, y*ystep) * 1 ;
			depthSSAO += min(0.01, depth - LinearizeDepth(texture2D( TexDepth , uv + delta ).r));
		}
	}
	depthSSAO -= nbPixels * depth;
	
	if(depthSSAO < 0) // IF = l'enfer
		depthSSAO = 0;
	if(depthSSAO > 0.02)
		depthSSAO = 0; 

	color.rgb /= 1+ (depthSSAO/20); 

	// color.rgb = mix(color.rgb, skyColor, pow(depth,0.7); //Brouillard 

	




    //Gamma correction
    color.r = pow(color.r,1.0/2.2);
    color.g = pow(color.g,1.0/2.2);
    color.b = pow(color.b,1.0/2.2);

	color_out = vec4(color.rgb,1.0);
}