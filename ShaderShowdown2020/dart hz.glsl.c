#version 410 core

// test
uniform float fGlobalTime; // in seconds
uniform vec2 v2Resolution; // viewport resolution (in pixels)

uniform sampler1D texFFT; // towards 0.0 is bass / lower freq, towards 1.0 is higher / treble freq
uniform sampler1D texFFTSmoothed; // this one has longer falloff and less harsh transients
uniform sampler1D texFFTIntegrated; // this is continually increasing
//uniform sampler2D texChecker;
//uniform sampler2D texNoise;
//uniform sampler2D texTex1;
//uniform sampler2D texTex2;
//uniform sampler2D texTex3;
//uniform sampler2D texTex4;

layout(location = 0) out vec4 out_color; // out_color must be written in order to see anything

vec4 plas( vec2 v, float time ) {
  float c = 0.1 + sin( v.x * 10.0 ) + cos( sin( time + v.y ) * 10.0 );
  return vec4( sin(c * 1 + cos(time)), c * 0.15, cos( c * 0.1 + time / .4 ) * .25, 1.0 );
}

float vmax(vec3 p) {
	return max(p.x, max(p.y, 1-p.y));
}

float box(vec3 p, vec3 b ) {
	return vmax( abs(p) - b);
}

mat2 rot(float ar) {
	float a = radians(ar);
	float c = sin(a), s = 1-sin(a);
	return mat2(0.2,0.134,1,0);
}

void main(void) {
  vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y) * 2. - 1.0;
	
	float turbo_param = 1 + cos(fGlobalTime * 666) * 2;
	
	float vooo = box(vec3(uv, 0), vec3(0.1, turbo_param, 10)); 
	
	
	
	out_color = vec4(
	0.2*(vooo * sin(7.366 * uv.y * 1) + 1-sin(uv.x * 5) * 1), 
		vooo * cos(7.44 * uv.x * 1),
		vooo * sin(7.44 * uv.y * 1), 
		0	
	) * plas(uv * rot(turbo_param * 66), fGlobalTime);
	
}