// Music: https://open.spotify.com/track/7qLxTxXhf6kqO2jTgnkyNH?si=jszNUZ7HS6moNeAoTXb-0A

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

float di(vec3 p) { return sin(p.x * 5.) * sin(p.y * 5.) * bass() * 0.001; }

mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }

float opD(vec3 p, float d)
{
    p.yz *= m(bass() * 0.15);
    p.xy *= m(bass() * 0.1);
    return di(p) + d;
}


float map(vec3 p, float t)
{
    return opD(p, length(p * p) - 1.0);
}

vec3 render(vec2 uv, float t, int iters)
{
    vec3 col = vec3(0);
    float d = 7.;

    for (int i = 0; i < iters; ++i)
    {
        vec3 p = vec3(0, 0, 8.) + normalize(vec3(uv, -1.)) * d;
        float d1 = map(p, t), d2 = map(p + 0.1, t);
        float f = clamp(d1 - d2, -0.1, 1.0);
        vec3 l = vec3(abs(sin(t)) * 1.0, (1.0 - abs(sin(t))) * 2.0, 3.0) + vec3(0.5, 0.3, 0.4) * f;
        vec2 v = uv.xy + vec2(sin(t));
        bool fx = true;//int(t) % 2 == 0;
        vec3 col_new = col * l + smoothstep(2.5, 0.0, d1) * 0.5;//* min(l, texture(texTex3, texture(texNoise, uv + vec2(fGlobalTime*0.1)).rg).rgb);
        if (!fx || all(!lessThan(fract(v * 80.), vec2(0.1, 0.1))))
            col = col_new;
        else
            col = col_new * 0.85;


        d += min(d1, 1.0);
    }

    return col;
}

vec3 render_fx(vec2 uv, float t)
{
    vec3 col;
    col += render(uv + vec2(0.1, -0.05), t, 3);
    col += render(uv + vec2(-0.1, -0.05), t, 2);
    col += render(uv + vec2(0.1, 0.05), t, 1);
    col /= 4.;
    return col;
}

void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);


    out_color.rgb = render_fx(uv, fGlobalTime);
}
