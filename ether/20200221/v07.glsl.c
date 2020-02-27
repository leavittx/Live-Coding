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
    float f;
    for (int i = 0; i < 64; ++i)
        f += texture(texFFTIntegrated, i / 1024.).r;
    return f;
}

float bass2()
{
    float f;
    for (int i = 0; i < 32; ++i)
        f += texture(texFFTSmoothed, i / 1024.).r;
    return f;
}

vec3 plas(vec2 v, float time)
{
    float c = 0.5 + sin(v.x * 10.0) + cos(sin(time + v.y) * 20.0);
    return vec3(sin(c * 0.2 + cos(time)), c * 0.15, cos(c * 0.1 + time / .4) * .25);
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    vec2 uv1 = uv;

    float d = mix(1., 1. + (1 - dot(uv, uv)), bass2() * 20.);
    uv *= d;

    vec3 tex = mix(texture(texTex3, uv).rgb, texture(texChecker, uv).rgb, smoothstep(0.0, 4.0, length(uv)));
    vec3 col = mix(tex, plas(uv1, bass() * 0.1), bass2() * 100.2);

    col += vec3(0.7, 0.8, 0.) - smoothstep(0., 4., length(uv));
    col *= 1. - 0.3 * dot(uv, uv);
    col *= pow(col, vec3(1.5));

    //col = 1.0*clamp(col, vec3(0), vec3(1));

    out_color = vec4(col, 1.);
}