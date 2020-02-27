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


vec3 tonemap(vec3 color)
{
    // Tonemapping and color grading
    color = pow(color, vec3(1.5));
    color = color / (1.0 + color);
    color = pow(color, vec3(1.0 / 1.5));
    color = mix(color, color * color * (3.0 - 2.0 * color), vec3(1.0));
    color = pow(color, vec3(1.3, 1.20, 1.0));
    color = clamp(color * 1.01, 0.0, 1.0);
    color = pow(color, vec3(0.7 / 2.2));
    return color;
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
    p.xz *= m(t * 2.8);//m(bass());
    p.xy *= m(t * 2.8);
    vec3 q = p * 2.0 + t;
    return length(p + vec3(sin(t * 0.7))) * log(length(p) + 1.0) +
        sin(q.x + sin(q.z + sin(q.y))) * 0.5 - 1.0;
}

float sdSphere(vec3 p, float s)
{
    return length(p) - s;
}

float sdBox(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float opU(float d1, float d2) { return min(d1, d2); }
float opS(float d1, float d2) { return max(-d1, d2); }
float opI(float d1, float d2) { return max(d1, d2); }

float map2(vec3 p)
{
    p.xy *= m(bass());
    return opS(sdBox(p, vec3(1)), sdSphere(p, 1.0));
}

float displacement(vec3 p)
{
    return sin(20 * p.x) * sin(20 * p.y) * sin(20 * p.z);
}

float map3(vec3 p)
{
    float d1 = map2(p);
    float d2 = displacement(p);
    return d1 + d2;
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0.);
    float d = 3.0;//* abs(sin(bass())); // Camera distance

    //int ITERS = int(10.0*(1.0+sin(fGlobalTime)));

    int ITERS = iters;
    for (int i = 0; i <= ITERS; ++i)
    {
        vec3 p = vec3(0, 0, 5.0) + normalize(vec3(uv, -1.0)) * d;
        float dist = min(map2(p), map3(p));
        float f = clamp((dist - map(p + .1)) * 0.5, -0.1, 1.0); // offset a bit
        vec3 l = vec3(0.1, 0.3, 0.4) + vec3(5.0, 2.5, 3.0) * f; // calc color
        col = col * l + smoothstep(2.5, 0.0, dist) * 0.7 * l;
        d += min(dist, 0.7);
    }
    return col;
}

vec3 render_iter(vec2 uv, float t)
{
    float iters = 5.;
    float dt = .5;

    vec3 acc = vec3(0.);
    for (float i = 0.; i < iters; i++) {
        //  offset render time for each color channel
        acc.r += render(uv + vec2(-0.05, 0.1), t + i * dt, 2).r;
        acc.g += render(uv + vec2(0.05, 0.1), t + 2. * i * dt, 7).g;
        acc.b += render(uv + vec2(-0.02, -0.1), t + 3. * i * dt, 3).b;
    }
    return acc / iters;
}

void main(void)
{
    vec2 uv = gl_FragCoord.xy / v2Resolution.y - vec2(.9, .5);

    vec3 col = render_iter(uv, fGlobalTime);

    //TODO: float bpm = 120.f;

    float h = 0.2;//abs(sin(fGlobalTime)*0.4);
    //col = vec3(1) - col;//smoothstep(col, vec3(1) - col, vec3(h));//mix(col, color, bass() / 2000.0);
    col = mix(col, vec3(1) - col, bass() / 100000.0);
    //out_color.rgb = col;
    out_color.rgb = tonemap(col);
}