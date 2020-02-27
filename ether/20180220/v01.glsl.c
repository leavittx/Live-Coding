// Music: The Bad Plus playlist

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


float bass()
{
    float x;
    for (int i = 0; i < 64; ++i)
        x += texture(texFFTIntegrated, i / 1024.).x;
    return x;
}

float di(vec3 p) { return sin(p.x * 5.) * sin(p.y * 5.) * bass(); }

ma2

float opD(vec3 p, float d)
{

    return di(p) + d;
}

float map(vec3 p, float t)
{
    return opD(p, length(p) - 1.0);
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0);
    float d = 5.;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1.)) * d;
        float d1 = map(p, t), d2 = map(p + 0.1, t);
        float f = clamp(d1 - d2, -0.1, 1.0);
        vec3 l = vec3(2.0, 1.0, 3.0) + vec3(0.3, 0.2, 0.1) * f;
        col = col * l + smoothstep(2.5, 0.0, d1) * 0.7 * l;

        d += min(d1, 1.0);
    }

    return col;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    out_color.rgb = render(uv, fGlobalTime, 5);
}