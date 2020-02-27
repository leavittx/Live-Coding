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
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
        float dist = map(p);
        float f = clamp(dist - map(p + .1), -0.1, 1.0);
        vec3 l = vec3(0.3, 0.1, 0.4) + vec3(2.0, 1.0, 3.0) * f;

        bool fx = int(fGlobalTime) % 2 == 0;
        vec2 v = uv.xx;
        if (!fx || any(lessThan(fract(v * 10), vec2(0.4))))
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

    vec3 col = render_fx(uv, fGlobalTime);

    out_col.rgb = col;
}