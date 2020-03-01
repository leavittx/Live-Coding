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

vec3 plas(vec2 v, float time)
{
    float c = 0.5 + sin(v.x * 10.0) + cos(sin(time + v.y) * 20.0);
    return vec3(sin(c * 0.2 + cos(time)), c * 0.15, cos(c * 0.1 + time / .4) * .25);
}


float bass()
{
    float f;
    for (int i = 0; i < 64; ++i)
        f += texture(texFFTIntegrated, i / 1024.).r;
    // Set depending on the volume
    return f * 1.;
}


float bass2()
{
    float f;
    for (int i = 0; i < 32; ++i)
        f += texture(texFFTSmoothed, i / 1024.).r;
    // Set depending on the volume
    return f * 30.;
}

float opD(float d, vec3 p)
{
    return d + sin(p.x * 5.) * sin(p.y * 5.) * bass() * 0.00003;
}

mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }

float map(vec3 p)
{
    p.yz *= m(sin(fGlobalTime));
    p.xy *= m(bass() * 0.00005);
    return opD(length(p) - 1.0, p);
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0);
    float d = 5 + abs(sin(t)) * 3;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
        float d1 = map(p), d2 = map(p + .1);
        float f = clamp((d1 - d2) * .5, -0.1, 1.0);
        vec3 l = vec3(0.3, 0.4, 0.5) + vec3(5., 3., 4.) * f;
        col = col * f + smoothstep(2.5, 0.0, d1) * l;
        d += min(d1, 1.);
    }
    return col;
}

vec3 render_fx(vec2 uv, float t)
{
    vec3 col = vec3(0);
    col += render(uv, t, 3) * 0.2;
    col += render(uv + vec2(0.1, -0.05) * bass2() * 3., t, 5) * vec3(10.0, 1.0, 1.0) * 0.2;
    col += render(uv + vec2(-0.1, -0.05) * bass2() * 3., t, 3) * vec3(1.0, 1.0, 10.0) * 0.2;
    return col * 0.3;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);


    float d = mix(1., 1. + (1 - dot(uv, uv)), bass2() * 1.5);
    uv *= d;

    vec3 col = render_fx(uv, fGlobalTime);
    vec3 tex = texture(texTex3, uv).rgb;

    out_color = vec4(col, 1.);
    //return;

    col = mix(tex, col, length(col) * 100.);

    col = mix(col, plas(uv, fGlobalTime), 0.5);
    col += vec3(0.5, 0.9, 0.2) - smoothstep(0., 100., length(uv) * 10.) * 14.7;
    col *= 1. - dot(uv, uv) * 0.7;
    col = pow(col, vec3(1.5));

    out_color = vec4(col, 1.);

}