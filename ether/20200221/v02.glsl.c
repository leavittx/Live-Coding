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

vec4 plas(vec2 v, float time)
{
    float c = 0.5 + sin(v.x * 10.0) + cos(sin(time + v.y) * 20.0);
    return vec4(sin(c * 0.2 + cos(time)), c * 0.15, cos(c * 0.1 + time / .4) * .25, 1.0);
}
mat2 m(float t) { float c = cos(t), s = sin(t); return mat2(c, -s, s, c); }
void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    float t = fGlobalTime;
    uv *= m(0.4 * cos(3.141592 * t * 0.1) * 1.5 * length(uv));
    uv *= 1.35;
    uv *= m(3.141592);

    if (length(uv) < 1.0)
    {
        float d = mix(1., 1. + (1. - dot(uv, uv)), 1.);
        uv = uv / d;
    }

    vec2 mtc = uv;
    mtc *= vec2(2.0, 0.2);
    mtc.xy -= 0.04 * normalize(mtc) * pow(length(mtc), 3.0);
    mtc /= vec2(2.0, 0.2);

    uv = 1.01 * 5.25 * mtc;

    vec3 col = texture(texTex3, uv).rgb;
    col -= vec3(-.7, -.85, .0) + smoothstep(.0, 4.0, length(uv));  // color correction gradualy from the center 
    col *= 1. - .7 * length(dot(uv, uv));                         // vigneting 
    col = pow(col, vec3(1.5));

    out_color.rgb = col;
}