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
  return vec4(0);
}
mat2 m(float a)
{
  float c = cos(a), s = sin(a);
  return mat2(c, -s, s, c);
}
float bass()
{
  float f = 0.0;
  for (int i = 0; i < 64; ++i)
    f += texture(texFFTIntegrated, float(i) / 1024.0).x;
  return f;
}
#define t fGlobalTime
float map(vec3 p)
{
  p.xz *= m(bass());
  p.xy *= m(t*2.8);
  vec3 q = p*2.0 + t;
  return length(p + vec3(sin(t*0.7))) * log(length(p) + 1.0) + 
         sin(q.x + sin(q.z + sin(q.y))) * 0.5 - 1.0;
}
void main(void)
{
  vec2 p = gl_FragCoord.xy / v2Resolution.y - vec2(.9, .5);
  vec3 cl = vec3(0.);
  float d = 2.5;
  
  
  int ITERS = 5;
  //int ITERS = int(10.0*(1.0+sin(fGlobalTime)));
  for (int i = 0; i <= ITERS; i++)
  {
    vec3 p = vec3(0, 0, 5.0) + normalize(vec3(p, -1.0)) * d;
    float rz = map(p);
    float f = clamp((rz - map(p + .1)) * 0.5, -0.1, 1.0);
    vec3 l = vec3(0.1, 0.3, 0.4) + vec3(5.0, 2.5, 3.0) * f;
    cl = cl*l + smoothstep(2.5, 0.0, rz) * 0.7 * l;
    d += min(rz, 1.0);
  }

  // Tonemapping and color grading
  vec3 color = cl;

  color = pow(color, vec3(1.5));
  color = color / (1.0 + color);
  color = pow(color, vec3(1.0 / 1.5));
  color = mix(color, color * color * (3.0 - 2.0 * color), vec3(1.0));
  color = pow(color, vec3(1.3, 1.20, 1.0));
  color = clamp(color * 1.01, 0.0, 1.0);
  color = pow(color, vec3(0.7 / 2.2));

  out_color.rgb = mix(cl, color, bass() / 2000.0);
  
  return;
  
  
  
  #if 0
  vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
  uv -= 0.5;
  uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

  vec2 m;
  m.x = atan(uv.x / uv.y) / 3.14;
  m.y = 1 / length(uv) * .2;
  float d = m.y;

  float f = texture( texFFT, d ).r * 100;
  m.x += sin( fGlobalTime ) * 0.1;
  m.y += fGlobalTime * 0.25;

  vec4 t = plas( m * 3.14, fGlobalTime ) / d;
  t = clamp( t, 0.0, 1.0 );
  out_color = f + t;
  #endif
}