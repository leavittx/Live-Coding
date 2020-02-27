// Based on https://www.shadertoy.com/view/4djBDR
// But lacks the fade out effect (no buffers in bonzomatic)

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


void main(void)
{
    vec2 uv = vec2(gl_FragCoord.x / v2Resolution.x, gl_FragCoord.y / v2Resolution.y);
    uv -= 0.5;
    uv /= vec2(v2Resolution.y / v2Resolution.x, 1);

    int iFrame = int(fGlobalTime * 30.0);
    vec4 f;
    vec2 g = gl_FragCoord.xy;
    f.xy = v2Resolution;
    vec2 v = (g + g - f.xy) / f.y * 3.;
    // Was in shadertoy: iChannel0 - gives nice fade out for the particles
    f *= texture(texTex3, g / f.xy) / length(f);
    g = vec2(iFrame + 2, iFrame);
    g = v - sin(g) * fract(fGlobalTime * .1 + 4. * sin(g)) * 3.;
    f += .1 / max(abs(g.x), g.y);

    out_color = vec4(f.xyz, 1.0);
}