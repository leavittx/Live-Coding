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

float sdSph(vec3 p, float r) { return length(p) - r; }

float distort(vec3 p) { return sin(10. * p.x) * sin(10. * p.y) * sin(bass() * 0.0000002 * fGlobalTime * p.z); }
float opD(vec3 p, float d) { return distort(p) + d; }
float opI(float x, float y) { return max(x, y); }
float opS(float x, float y) { return max(x, -y); }

mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }

float tri(float x) { return abs(2.0 * fract(x) - 1.0); }

float map(vec3 p, float t)
{
    bool fx = int(t) % 3 == 0;
    p.xy *= m(tri(bass() * 0.05) * 1);
    //return opI(sdSph(p, 1.0), sdSph(p + 0.1, 2.7));
    return opD(p, opI(sdSph(p, 1.0), sdSph(p + 0.1, 0.7)));
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0);
    float d = 3;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 5) + normalize(vec3(uv, -1)) * d;
        float d1 = map(p, t);
        float d2 = map(p + .1, t);
        float f = clamp(d1 - d2, -0.1, 1.0);
        vec3 l = vec3(0.3 + abs(sin(t * 2)) * 0.3, 0.1, 0.2) + 0.5 * vec3(2.0, 1.0, 1.0) * f;
        vec2 v = p.xy;

        if (all(lessThan(fract(v * 8), vec2(abs(tri(t)) + 0.1, 0.1 * abs(tri(t + 1.5)) + 0.1))))
            col = col * l + smoothstep(2.5, 0., d1) * l * 0.7;
        else if (int(t) % 5 < 2)
            col = col * l + smoothstep(2.5, 0., d2 + d1) * l * 0.7;
        d += min(d1, 1.0);
    }

    return col;
}

vec3 render_fx(vec2 uv, float t)
{
    vec3 col;
    col += render(uv + vec2(-0.3, 0.05), t, 7);
    col += render(uv + vec2(0.3, 0.07), t, 9);
    col += render(uv + vec2(-0.1, -0.05), t, 3);
    return col;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);
    //uv += -vec2(0.3, 0.2);

    vec2 mtc = (gl_FragCoord.xy - 0.5 * v2Resolution.xy) / v2Resolution.y;
    mtc *= vec2(2., 0.2);
    mtc.xy -= 0.04 * normalize(mtc) * pow(length(mtc), 3.0);
    mtc /= vec2(2.0, 0.2);


    out_color.rgb = render(uv, fGlobalTime, 3);
    out_color.rgb = pow(out_color.rgb, vec3(0.45));
}