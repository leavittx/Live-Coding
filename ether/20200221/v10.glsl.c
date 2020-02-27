// Track: Watergate 20 ~47 min

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

float opD(vec3 p, float d)
{
    return d + sin(p.x * 10.) * sin(p.y * 10.) * bass2() * 2.;
}



float sdSph(vec3 p, float r)
{
    return length(p) - r;
}

//float sdBox(vec3 p, vec3 b)
//{
//    vec3 q = abs(p) - b;
//    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
//}


mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }

float map(vec3 p)
{
    float t = fGlobalTime;
    p.xz *= m(t * 2.8);//m(bass());
    p.xy *= m(t * 2.8);
 /*   vec3 q = p * 2.0 + t;
    float de = length(p + vec3(sin(t * 0.7))) * log(length(p) + 1.0) +
        sin(q.x + sin(q.z + sin(q.y))) * 0.5 - 1.0;*/

    p.yz *= m(bass() * 0.01);
    p.xz *= m(fGlobalTime);
    return opD(p, sdSph(p, 1.));
    //return opD(p, sdBox(p, vec3(0.3)));
    //float d = opOnion(sdSph(p, 1.), 0.01);
    //return opD(p, d);
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0);
    float d = 5;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
        float d1 = map(p), d2 = map(p + 0.1);
        float f = clamp((d1 - d2) * 0.5, -0.1, 1.0);
        vec3 l = vec3(0.3, 0.4, 0.5) + vec3(5.4, 5.3, 5.5) * f;
        col = col * f + smoothstep(2.5, 0.0, d1) * l * 0.7;
        d += min(d1, 1.0);
    }
    return col;
}

vec3 render_fx(vec2 uv, float t)
{
    vec3 col;
    col += render(uv + vec2(0.0, 0.0), t, 10) * vec3(10.0, 1.0, 1.0) / 3.;
    col += render(uv + vec2(-0.15, -0.1) * bass2() * 30.0, t, 5) * vec3(2.0, 0.0, 10.0) / 3.;
    col += render(uv + vec2(0.15, 0.1) * bass2() * 30.0, t, 5) * vec3(2.0, 0.0, 00.0) / 3.;
    return col / 3.;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    float d = mix(1., 1. + (1. - dot(uv, uv)), bass2() * 55.);
    uv *= d;

    vec3 col = render_fx(uv, fGlobalTime);
    vec3 tex = texture(texTex3, uv).rgb;
    col = mix(tex, col, length(col) * 1000);

    col += vec3(0.7, 0.8, 0.0) - smoothstep(0., 4., length(uv));
    col = mix(col, plas(uv, fGlobalTime), bass2());
    col *= 1 - dot(uv, uv) * 0.7;
    col = pow(col, vec3(1.5));

    out_color = vec4(col, 1.0);
}