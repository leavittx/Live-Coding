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

float sdSph(vec3 p, float r) { return length(p) - r; }

float opU(float x, float y) { return min(x, y); }
float opI(float x, float y) { return max(x, y); }
float opS(float x, float y) { return max(x, -y); }

float distort(vec3 p) { return sin(p.x * 20) * sin(p.y * 20) * sin(p.z * abs(sin(fGlobalTime * 0.2))); }
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

mat2 m(float t)
{
    float s = sin(t), c = cos(t);
    return mat2(c, s, -s, c);
}

#define t fGlobalTime
float mapE(vec3 p)
{
    p.xz *= m(t * 2.8);//m(bass());
    p.xy *= m(t * 2.8);
    vec3 q = p * 2.0 + t;
    return length(p + vec3(sin(bass() * 0.7))) * log(length(p) + 1.0) +
        sin(q.x + sin(q.z + sin(q.y))) * 0.5 - 1.0;
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0.);
    float d = 5;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d; // (0, 0, 7) also gives interesting result
        bool fx1 = int(t) % 3 == 0;
        bool fx11 = int(t) % 3 == 1;
        float dist1, dist2;
        //if (fx1)
        //    dist1 = mapE(p), dist2 = mapE(p + .1);
        //else if (fx11)
        dist1 = map(p) - mapE(p), dist2 = map(p + .1) - mapE(p + .1);
        //else
        //    dist1 = map(p), dist2 = map(p + .1);
        float dist = dist1;
        float f = clamp(dist - dist2, -0.1, 1.0);
        vec3 l = vec3(0.3, 0.1, 0.4) + vec3(2.0, 1.0, 3.0) * f;

        bool fx2 = int(t) % 2 == 0;
        vec2 v = uv.xx;
        //if (!fx2 || any(lessThan(fract(v * 10), vec2(0.4))))
        col = col * l + smoothstep(2.5, 0.0, dist) * 0.7 * l;
        d += min(dist, 1.0);


        /*vec2 v = uv.xx;
        if (int(fGlobalTime) % 2 == 0)
          v = uv.yy;
        if (all(lessThan(fract(v*sin(bass())*10000), vec2(0.8))))
        {
          col.g += dist*sin(dist);
          col.b += dist;
        }
        else
        {
          col.r += dist;
        }*/

        /*col.r += dist;
        col.g += sin(dist*dist);
        col.b += dist * length(dist*dist);*/
    }

    return col;
}

vec3 render_fx(vec2 uv, float t)
{
    vec3 acc = vec3(0.);

    acc += render(uv + vec2(-0.2, 0.05), t, 3);

    acc += render(uv + vec2(0.25, -0.05), t, 5);

    acc += render(uv + vec2(-0.25, 0.05), t, 1);

    acc /= 1.0;

    return acc;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    vec2 uv1 = uv;

    float d = mix(1.0, 1 + (1 - dot(uv, uv)), bass2() * 20.);
    uv = uv * d;

    float fft = bass2() * 20.0;
    float plas_t = bass() * 0.05;
    vec3 tex = mix(texture(texTex3, uv).rgb, texture(texChecker, uv).rgb, smoothstep(.0, 4.0, length(uv)));
    vec3 col = mix(tex, plas(uv1, plas_t), fft);

    col += vec3(.7, .85, .0) - smoothstep(.0, 4.0, length(uv));  // color correction gradualy from the center 
    col *= 1. - .7 * dot(uv, uv);                         // vigneting 
    col = pow(col, vec3(1.5));

    //col = 1.0*clamp(col, vec3(0), vec3(1));

    out_color.rgb = col;
}