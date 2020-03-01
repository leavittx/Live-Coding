#version 410 core

uniform float fGlobalTime; // in seconds
uniform vec2 v2Resolution; // viewport resolution (in pixels)

uniform sampler1D texFFT; // towards 0.0 is bass / lower freq, towards 1.0 is higher / treble freq
uniform sampler1D texFFTSmoothed; // this one has longer falloff and less harsh transients
uniform sampler1D texFFTIntegrated; // this is continually increasing
uniform sampler2D texChecker;
uniform sampler2D texNoise;
uniform sampler2D texTex1;
uniform sampler2D texTex2;
uniform sampler2D texTex3;
uniform sampler2D texTex4;

layout(location = 0) out vec4 out_color; // out_color must be written in order to see anything

vec4 plas( vec2 v, float time )
{
  float c = 0.5 + sin( v.x * 10.0 ) + cos( sin( time + v.y ) * 20.0 );
  return vec4( sin(c * 0.2 + cos(time)), c * 0.15, cos( c * 0.1 + time / .4 ) * .25, 1.0 );
}

mat2 rot(vec2 p, float a)
{
	float a1= radians(a);
	float c = cos(a1), s = sin(a1);
	return mat2(c,-s,s,c);
}

float vmax(vec3 p)
{
	return max(p.x, max(p.y, p.z));
}

float box (vec3 p, vec3 b)
{
		return vmax( abs(p) - b);
}
float sp(vec3 p, float r)
{
	return length(p) - r; 
}
float mod1(float m, float p) 
{
	return mod(m - p * 0.5, p) + p*.5;
}

vec2 mod2(vec2 m, vec2 p) 
{
	return mod(m , p) + p*.5;
}


vec2 modp(vec2 p, float a0)
{
	float a = atan(p.y, p.x);
	float l = length(p);
			float a1 = radians(a0);

	a = mod( a - a1*0.5, a1) + a1*0.5; 
	return vec2( cos(a), sin(a) ) * l;
}

float scene(vec3 p)
{
	vec3 p0 = p;

	p0.xz *= rot(p.xz, fGlobalTime * 40.);
	
//	p0.xz = mod2(p0.xz, vec2(0.7));
	
//	p0.zx = modp(p0.zx, 30);
	
	p0.xz *= rot(p.xz, p.y * sin(fGlobalTime*0.4)*120.);
	
	float b = box(p0, vec3(0.8));
	float s = sp(p0, 1.1);
	
	return max(-s,b);
}


void main(void)
{
  vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y) * 2. - 1.;
//  uv -= 0.5;
  uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

	
	vec3 ro = vec3(0,0,-8);
	vec3 rd = normalize(vec3(uv.x, uv.y, 1.));
	
	float t =0;
	float shad = 0.0;
	for (int i = 0; i < 100; ++ i) {
		vec3 p = ro + rd * t;
		float dt = scene(p);
		t += dt;
		if ( dt < 0.001) {
			shad = 1. - float(i) / 100.;
			break;
		}
	}
	
	
	float t1 = shad * shad + 10;
	
	vec3 col = vec3(0.5) + vec3(0.5) * cos(6.28*vec3(1.,0.7,0.4) * t1 + vec3(0,0.15,0.2) );
	
  out_color = vec4(col*t,1);
}