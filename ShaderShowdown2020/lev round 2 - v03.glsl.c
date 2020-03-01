#version 410 core
//machine2
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

vec3 plas( vec2 v, float time )
{
  float c = 0.5 + sin( v.x * 10.0 ) + cos( sin( time + v.y ) * 20.0 );
  return vec3( sin(c * 0.2 + cos(time)), c * 0.15, cos( c * 0.1 + time / .4 ) * .25);
}

float bass()
{
  float f;
  for (int i = 0; i < 64; ++i)
    f+= texture(texFFTIntegrated, i / 1024.).x;
  // set depending on the volume
  return f * 2.;
}

float bass2()
{
  float f;
  for (int i = 0; i < 32; ++i)
    f+= texture(texFFTSmoothed, i / 1024.).x;
  // set depending on the volume
  return f * 30.;
}

float sdSph(vec3 p, float r) { return length(p) - r; }

float opU(float x, float y) { return min(x, y); }

float opD(vec3 p, float d)
{
  return d + sin(p.x * 10.) * sin(p.z * 10.) * bass() * 0.00002;
}

mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }

float map(vec3 p)
{
  //p += sin(fGlobalTime);
  p.xy *= m(bass() * 0.05);
  p.zx *= m(bass2() * 0.01);
  float d = opU(sdSph(p, 1.), sdSph(p + 0.1, 0.33));
  return opD(p, d);
}

vec3 render(vec2 uv, float t, int iters)
{
  vec3 col = vec3(0);
  float d = 5;
  
  for (int i = 0; i < iters; ++i)
  {
    vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
    float d1 = map(p), d2 = map(p + .1);
    float f = clamp((d1-d2)*.5, -0.1, 1.0);
    vec3 l = vec3(0.4, 0.5, 0.8) * f + vec3(5.0, 3.0, 5.0) * f;
    
    bool fx = int(t) % 2 == 0;
    vec2 v = uv.xy;
    if ( !fx || all( lessThan(fract(v * 100.), vec2(0.7, 0.7)) ) )
      col = col * l + smoothstep(2.5, 0.0, d1) * l * 0.7;
    d += min(d1, 0.);    
  }
  return col;
}

vec3 render_fx(vec2 uv, float t)
{
  vec3 col = vec3 (0);
  
  col += render(uv, t, 7) * 4.;
  col += render(uv + vec2(0.1, 0.1), t, 7);
  col += render(uv + vec2(-0.1, 0.1), t, 7);
  
  return col * 0.1;
}


void main(void)
{
  vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
  uv -= 0.5;
  uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

  float d = mix(1., 1. + (1 - dot(uv, uv)), bass2()* 0.55);
  uv *= d;
  
  //vec3 col = render(uv, fGlobalTime, 7);
  vec3 col = render_fx(uv, fGlobalTime);
  col = mix(col, plas(uv, fGlobalTime), bass2() * 0.1);
  
  col += (vec3(0.7, 0.8, 0.0) + smoothstep(0., 4., dot(uv, uv))) * bass2()*0.3;
  col *= 1. - dot(uv, uv);
  
  out_color = vec4(col, 1.);
}