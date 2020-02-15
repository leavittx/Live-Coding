// https://www.shadertoy.com/view/llfSzH
//166 chars by Fabrice

#define mainImage(f, u) \
    vec3 R = iResolution, r = cos(iTime+R/R.y); \
    for(float i = .6, a; \
        i > .1 && (a = length(fract(r)-.5)-.3) > .001; \
        i -= .002) \
        r += vec3((u+u-R.xy)/R.y,2)*.06*a, \
        f.bgr = i+i*r //
//
//
//182 chars, WebGL 2.0
/*
void mainImage (out vec4 f, vec2 u)
{
    vec3 R = iResolution, r = cos(iDate.w+R/R.y);

    for(float i = .6, a; i > .1 && (a = length(fract(r)-.5)-.3) > .001; i -= .002)

        f.bgr = i+i*(r += vec3((u+u-R.xy)/R.y,2)*.06*a);
}
/**/ //
//
//185 chars, 834144373's path
//191 chars, Fabrice's idea, but I changed the constants a little
//198 chars, Fabrice: if() -> ?:
//199 chars, additional size optimizations by Nrx and aiekick
//
/*
void mainImage (out vec4 f, vec2 u)
{
    //various 3d camera paths
    //vec3 r = vec3 (cos (f=iDate).w, 1, sin (f.w)), R = iResolution; //198 original
    //vec3 r = cos(iDate.w+vec3(1,2,3)), R = iResolution; //191 Fabrice and me
    vec3 R = iResolution,r = cos(iDate.w+R/R.y); //185 834144373

    for (float i = .6 ; i > .1 ; i -= .002)
        (f.a = length (fract (r) - .5) - .3) > .001
        ? f.bgr = i + i * (r += vec3 ((u + u - R.xy) / R.y, 2) * .06 * f.a)
        : r;
}
/**/ //
//
//210, my original (with a little help by FabriceNeyret2 ;)
/*
void mainImage( out vec4 f, vec2 u )
{
    vec3 r = vec3(cos(iDate.w), 1, sin(iDate.w)) + 1.,
         R = iResolution ;

    for( float i = .6 ; i > .1 ; i-=.002 ) {
        r += vec3( (u+u-R.xy)/R.y, 2 ) * .06
             * ( f.a = length( fract(r) - .5 ) - .3 ) ;
        f.bgr=i*r;
        if( f.a < .001 ) break ;
    }

}
/**/