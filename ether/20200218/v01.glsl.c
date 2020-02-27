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

layout(location = 0) out vec4 out_col; // out_color must be written in order to see anything

float bass()
{
    float r;
    for (int i = 0; i < 64; ++i)
        r += texture(texFFTIntegrated, i / 1024.0).r;
    return r;
}

float sdSph(vec3 p, float r) { return length(p) - r; }

float opU(float x, float y) { return min(x, y); }
float opI(float x, float y) { return max(x, y); }
float opS(float x, float y) { return max(x, -y); }

float distort(vec3 p) { return sin(p.x * 20) * sin(p.y * 20) * sin(p.z * 1000 * abs(sin(bass() * fGlobalTime * 0.2))); }
float opD(vec3 p, float x)
{
    return distort(p) + x;
}

float map(vec3 p)
{
    return opS(
        sdSph(p, 3.f),
        opD(
            p, sdSph(p - vec3(0.7) * sin(bass()), 2.5))); //////////// <- change the last parameter here!!!
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0.);
    float d = 5;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
        float dist = map(p);
        col.r += dist;
        col.g += sin(dist * dist);
        col.b += dist * length(dist * dist);
    }

    return col;
}


void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    out_col.rgb = render(uv, fGlobalTime, 5);
}