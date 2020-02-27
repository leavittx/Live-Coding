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
mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }
void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    vec2 uv1 = uv;

    if (length(uv) < 1.)
    {
        float d = mix(1., 1. + (1. - dot(uv, uv)), bass2() * 20.);
        uv = uv * d;
    }

    float fft = bass2() * 20.0;
    float plas_t = bass() * 0.05;
    vec3 tex = mix(texture(texTex3, uv).rgb, texture(texTex4, uv).rgb, 0.0);
    vec3 col = mix(tex, plas(uv1, plas_t), fft);
    col -= vec3(-.7, -.85, .0) + smoothstep(.0, 4.0, length(uv));  // color correction gradualy from the center 
    col *= 1. - .7 * length(dot(uv, uv));                         // vigneting 
    col = pow(col, vec3(1.5));

    out_color.rgb = col;
}