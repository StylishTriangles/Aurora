//afl_ext 2017

// its fast because it doesnt use FBM noise
// 4k 60 fps on gtx 1070

#ifdef GL_ES
precision highp float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
varying vec2 surfacePosition;
// Generally all the code comes from https://github.com/achlubek/venginenative/ which you can star if you like it!

//#define time iGlobalTime
//#define mouse (iMouse.xy / iResolution.xy)
//#define resolution iResolution.xy

// lower this if your GPU cries for mercy (set to 0 to remove clouds!)
#define CLOUDS_STEPS 120
#define ENABLE_SSS 1

// its from here https://github.com/achlubek/venginenative/blob/master/shaders/include/WaterHeight.glsl
float wave(vec2 uv, vec2 emitter, float speed, float phase){
        float dst = distance(uv, emitter);
        return pow((0.5 + 0.5 * sin(dst * phase - time * speed)), 5.0);
}

#define GOLDEN_ANGLE_RADIAN 2.39996
float getwaves(vec2 position){
        position *= 0.1;
        float w = wave(position, vec2(70.3, 60.4), 1.0, 4.5) * 0.5;
        w += wave(position, vec2(60.3, -55.4), 1.0, 5.0) * 0.5;

        w += wave(position, vec2(-74.3, 50.4), 1.0, 4.0) * 0.5;
        w += wave(position, vec2(-60.3, -70.4), 1.0, 4.7) * 0.5;


        w += wave(position, vec2(-700.3, 500.4), 2.1, 8.0) * 0.1;
        w += wave(position, vec2(700.3, -500.4), 2.4, 8.8) * 0.1;

        w += wave(position, vec2(-700.3, -500.4), 2.6, 9.0) * 0.1;
        w += wave(position, vec2(-700.3, -500.4), 2.7, 9.6) * 0.1;

        w += wave(position, vec2(300.3, -760.4), 2.0, 12.0) * 0.08;
        w += wave(position, vec2(-300.3, -400.4), 2.230, 13.0) * 0.08;

        w += wave(position, vec2(-100.3, -760.4), 2.0, 14.0) * 0.08;
        w += wave(position, vec2(-100.3, 400.4), 2.230, 15.0) * 0.08;

        w += wave(position, vec2(300.3, -760.4), 2.0, 22.0) * 0.08;
        w += wave(position, vec2(-300.3, -400.4), 2.230, 23.0) * 0.08;

        w += wave(position, vec2(-100.3, -760.4), 2.0, 24.0) * 0.08;
        w += wave(position, vec2(-100.3, 400.4), 2.230, 22.0) * 0.08;


    return w * 0.44;
}



float hashx( float n ){
    return fract(sin(n)*758.5453);
}

float noise2X( vec2 x2 , float a, float b, float off){
        vec3 x = vec3(x2.x, x2.y,time * 0.1 + off);
        vec3 p = floor(x);
        vec3 f = fract(x);
        f       = f*f*(3.0-2.0*f);

        float h2 = a;
         float h1 = b;
        #define h3 (h2 + h1)

         float n = p.x + p.y*h1+ h2*p.z;
        return mix(mix(	mix( hashx(n+0.0), hashx(n+1.0),f.x),
                        mix( hashx(n+h1), hashx(n+h1+1.0),f.x),f.y),
                   mix(	mix( hashx(n+h2), hashx(n+h2+1.0),f.x),
                        mix( hashx(n+h3), hashx(n+h3+1.0),f.x),f.y),f.z);
}

float supernoise(vec2 x){
    float a = noise2X(x, 55.0, 92.0, 0.0);
    float b = noise2X(x + 0.5, 133.0, 326.0, 0.5);
    return (a * b);
}
float supernoise3d(vec3 x);

#define cosinelinear(a) ((cos(a * 3.1415) * -sin(a * 3.1415) * 0.5 + 0.5))
#define snoisesinpow(a,b) pow(1.0 - abs(supernoise3d(vec3(a, Time)) - 0.5) * 2.0, b)
#define XX(a,b) pow(1.0 - abs((a) - 0.5) * 2.0, b)
#define snoisesinpow2(a,b) pow(cosinelinear(supernoise(a)), b)
#define snoisesinpow3(a,b) pow(1.0 - abs(supernoise(a ) - 0.5) * 2.0, b)
#define snoisesinpow4X(a,b) pow(1.0 - 2.0 * abs(supernoise(a) - 0.5), b)
#define snoisesinpow4(a,b) pow(cosinelinear(1.0 - 2.0 * abs(supernoise(a) - 0.5)), b)
#define snoisesinpow5(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)
#define snoisesinpow6(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)

float heightwaterHI2(vec2 pos){
    float res = 0.0;
    pos *= 6.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 6.0;
    float tmod = 5210.1;

    for(int i=0;i<6;i++){
        vec2 t = vec2(tmod * time*0.00018);
        float x1 = snoisesinpow4X(pos + t, chop);
        float x2 = snoisesinpow4(pos + t, chop);
        res += wz * mix(x1 * x2, x2, supernoise(pos + t) * 0.5 + 0.5) * 2.5;
        w += wz * 1.0;
        x1 = snoisesinpow4X(pos - t, chop);
        x2 = snoisesinpow4(pos - t, chop);
        res += wz * mix(x1 * x2, x2, supernoise(pos - t) * 0.5 + 0.5) * 2.5;
        w += wz * 1.0;
        chop = mix(chop, 5.0, 0.3);
        wz *= 0.4;
        pos *= vec2(2.1, 1.9);
        tmod *= 0.8;
    }
    w *= 0.55;
    return (pow(res / w * 2.0, 1.0));
}
float getwavesHI(vec2 uv, float details){
        return (getwaves(uv)) + details * 0.09 * heightwaterHI2(uv * 0.1  );
}

float H = 0.0;
vec3 normal(vec2 pos, float e, float depth){
    vec2 ex = vec2(e, 0);
    float inf = 1.0 ;
    H = getwavesHI(pos.xy, inf) * depth;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(cross(normalize(a-vec3(pos.x - e, getwavesHI(pos.xy - ex.xy, inf) * depth, pos.y)),
                           normalize(a-vec3(pos.x, getwavesHI(pos.xy + ex.yx, inf) * depth, pos.y + e))));
}
mat3 rotmat(vec3 axis, float angle)
{
        axis = normalize(axis);
        float s = sin(angle);
        float c = cos(angle);
        float oc = 1.0 - c;

        return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}
vec3 getRay(vec2 uv){
   uv = (uv * 2.0 - 1.0)* vec2(resolution.x / resolution.y, 1.0);
        vec3 proj = normalize(vec3(uv.x, uv.y, 1.0) + vec3(uv.x, uv.y, -1.0) * pow(length(uv), 2.0) * 0.05);
    if(resolution.y < 400.0) return proj;
        vec3 ray = rotmat(vec3(0.0, -1.0, 0.0), 3.0 * (mouse.x * 2.0 - 1.0)) * rotmat(vec3(1.0, 0.0, 0.0), 1.5 * (mouse.y * 2.0 - 1.0)) * proj;
    return ray;
}

float rand2sTime(vec2 co){
    co *= time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float raymarchwater2(vec3 camera, vec3 start, vec3 end, float depth){
    float stepsize = 1.0 / 6.0;
    float iter = 0.0;
    vec3 pos = start;
    float h = 0.0;
    for(int i=0;i<7;i++){
        pos = mix(start, end, iter);
        h = getwaves(pos.xz) * depth - depth;
        if(h > pos.y) {
            return distance(pos, camera);
        }
        iter += stepsize;
    }
    return -1.0;
}

float raymarchwater(vec3 camera, vec3 start, vec3 end, float depth){
    float stepsize = 1.0 / 66.0;
    float iter = 0.0;
    vec3 pos = start;
    float h = 0.0;
    for(int i=0;i<67;i++){
        pos = mix(start, end, iter);
        h = getwaves(pos.xz) * depth - depth;
        if(h > pos.y) {
            return raymarchwater2(camera, mix(start, end, iter - stepsize), mix(start, end, iter), depth);
        }
        iter += stepsize;
    }
    return -1.0;
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{
    return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
}
#define PI 3.141592
#define iSteps 16
#define jSteps 8

vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

float hash( float n ){
    return fract(sin(n)*758.5453);
    //return fract(mod(n * 2310.7566730, 21.120312534));
}

float noise3d( in vec3 x ){
        vec3 p = floor(x);
        vec3 f = fract(x);
        f       = f*f*(3.0-2.0*f);
        float n = p.x + p.y*157.0 + 113.0*p.z;

        return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
                        mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
                   mix(	mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float noise2d( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(mix(hash(n+0.0),hash(n+1.0),f.x),mix(hash(n+57.0),hash(n+58.0),f.x),f.y);
}
 float configurablenoise(vec3 x, float c1, float c2) {
        vec3 p = floor(x);
        vec3 f = fract(x);
        f       = f*f*(3.0-2.0*f);

        float h2 = c1;
         float h1 = c2;
        #define h3 (h2 + h1)

         float n = p.x + p.y*h1+ h2*p.z;
        return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
                        mix( hash(n+h1), hash(n+h1+1.0),f.x),f.y),
                   mix(	mix( hash(n+h2), hash(n+h2+1.0),f.x),
                        mix( hash(n+h3), hash(n+h3+1.0),f.x),f.y),f.z);

}

float supernoise3d(vec3 p){

        float a =  configurablenoise(p, 883.0, 971.0);
        float b =  configurablenoise(p + 0.5, 113.0, 157.0);
        return (a + b) * 0.5;
}
float supernoise3dX(vec3 p){

        float a =  configurablenoise(p, 883.0, 971.0);
        float b =  configurablenoise(p + 0.5, 113.0, 157.0);
        return (a * b);
}
float fbmHI(vec3 p){
   // p *= 0.1;
    p *= 0.0000169;
    p.x *= 0.489;
        p += time * 0.02;
        //p += getWind(p * 0.2) * 6.0;
        float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
        for(int i=0;i<3;i++){
        //p += noise(vec3(a));
                a += clamp(2.0 * abs(0.5 - (supernoise3dX(p))) * w, 0.0, 1.0);
                wc += w;
        w *= 0.5;
                p = p * 3.0;
        }
        return a / wc;// + noise(p * 100.0) * 11;
}
#define MieScattCoeff 2.0
vec3 wind(vec3 p){
    return vec3(
        supernoise3d(p),
        supernoise3d(p.yzx),
        supernoise3d(-p.xzy)
        ) * 2.0 - 1.0;
}
struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };

float planetradius = 6378000.1;
float minhit = 0.0;
float maxhit = 0.0;
float rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    vec2 ex = vec2(-b - sqrt(disc), -b + sqrt(disc))/2.0;
    minhit = min(ex.x, ex.y);
    maxhit = max(ex.x, ex.y);
    return mix(ex.y, ex.x, step(0.0, ex.x));
}
vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);
        float rs = rsi2(Ray(r0, r), Sphere(vec3(0), rAtmos));
        vec3 px = r0 + r * rs;
shMie *= ( (pow(fbmHI(px  ) * (supernoise3dX(px* 0.00000669 + time * 0.001)*0.5 + 0.5) * 1.3, 3.0) * 0.8 + 0.5));

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}
vec3 getatm(vec3 ray){
        vec3 sd = rotmat(vec3(1.0, 1.0, 0.0), time * 0.25) * normalize(vec3(0.0, 1.0, 0.0));
    vec3 color = atmosphere(
        ray,           // normalized ray direction
        vec3(0,6372e3,0),               // ray origin
        sd,                        // position of the sun
        22.0,                           // intensity of the sun
        6371e3,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        8e3,                            // Rayleigh scale height
        1.2e3 * MieScattCoeff,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    );
        return color;

}

float sun(vec3 ray){
        vec3 sd = rotmat(vec3(1.0, 1.0, 0.0), time * 0.25) * normalize(vec3(0.0, 1.0, 0.0));

    return pow(max(0.0, dot(ray, sd)), 228.0) * 110.0;
}
float smart_inverse_dot(float dt, float coeff){
    return 1.0 - (1.0 / (1.0 + dt * coeff));
}
#define VECTOR_UP vec3(0.0,1.0,0.0)
vec3 getSunColorDirectly(float roughness){
    vec3 sunBase = vec3(15.0);
        vec3 sd = rotmat(vec3(1.0, 1.0, 0.0), time * 0.25) * normalize(vec3(0.0, 1.0, 0.0));

    float dt = max(0.0, (dot(sd, VECTOR_UP)));
    float dtx = smoothstep(-0.0, 0.1, dt);
    float dt2 = 0.9 + 0.1 * (1.0 - dt);
    float st = max(0.0, 1.0 - smart_inverse_dot(dt, 11.0));
    vec3 supersundir = max(vec3(0.0),   vec3(1.0) - st * 4.0 * pow(vec3(50.0/255.0, 111.0/255.0, 153.0/255.0), vec3(2.4)));
//    supersundir /= length(supersundir) * 1.0 + 1.0;
    return supersundir * 4.0 ;
    //return mix(supersundir * 1.0, sunBase, st);
    //return  max(vec3(0.3, 0.3, 0.0), (  sunBase - vec3(5.5, 18.0, 20.4) *  pow(1.0 - dt, 8.0)));
}
#define xsupernoise3d(a) abs(0.5 - noise3d(a))*2.0
#define xsupernoise3dx(a) abs(0.5 - supernoise3d(a))*2.0
float fbmHxI(vec3 p){
   // p *= 0.1;
    p *= 0.021;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<3;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.6;
        p = p * 3.2  ;
    }
    return a;
}float fbmHxI2(vec3 p){
   // p *= 0.1;
    p *= 0.591;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<3;i++){
        //p += noise(vec3(a));
        a += xsupernoise3dx(p) * w;
        w *= 0.5;
        p = p * 3.6  ;
    }
    return a;
}
#define CloudsFloor 3000.0
#define CloudsCeil 6000.0

float getHeightOverSea(vec3 p){
    vec3 atmpos = vec3(0.0, planetradius, 0.0) + p;
    return length(atmpos) - planetradius;
}
vec2 cloudsDensity3D(vec3 pos){
    vec3 ps = pos;

    vec3 p = ps * 0.005;
    float density = fbmHxI( p + fbmHxI2(p * 0.8 + 0.1 * time) * 9.5 + time );

    float measurement = (CloudsCeil - CloudsFloor) * 0.5;
    float mediana = (CloudsCeil + CloudsFloor) * 0.5;
    float h = getHeightOverSea(pos);
    float mlt = (( 1.0 - (abs( h - mediana ) / measurement )));
    float init = smoothstep(0.25, 0.35, density * mlt);
    return  vec2(init, (h - CloudsFloor) / (CloudsCeil - CloudsFloor)) ;
}

vec2 UV = vec2(0.0);
vec4 internalmarchconservative(vec3 atm, vec3 p1, vec3 p2, float noisestrength){
        int stepsmult = int(abs(CloudsFloor - CloudsCeil) * 0.001);
    int stepcount = CLOUDS_STEPS;
    float stepsize = 1.0 / float(stepcount);
    float rd = fract(rand2sTime(UV)) * stepsize * noisestrength;
    float c = 0.0;
    float w = 0.0;
    float coverageinv = 1.0;
    vec3 pos = vec3(0);
    float clouds = 0.0;
    vec3 color = vec3(0.0);
        float colorw = 1.01;
    float godr = 0.0;
    float godw = 0.0;
    float depr = 0.0;
    float depw = 0.0;
    float iter = 0.0;
    vec3 lastpos = p1;
    //depr += distance(CAMERA, lastpos);
    depw += 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    for(int i=0;i<CLOUDS_STEPS;i++){
        pos = mix(p1, p2, iter + rd);
        vec2 as = cloudsDensity3D(pos);
        clouds = as.x;
        float W = clouds * max(0.0, coverageinv);
        color += W * vec3(as.y * as.y);
        colorw += W;

        coverageinv -= clouds;
        depr += step(0.99, coverageinv) * distance(lastpos, pos);
        if(coverageinv <= 0.0) break;
        lastpos = pos;
        iter += stepsize;
        //rd = fract(rd + iter * 124.345345);
    }
    if(coverageinv > 0.99) depr = 0.0;
    float cv = 1.0 - clamp(coverageinv, 0.0, 1.0);
    color *= getSunColorDirectly(0.0) * 5.0;
        vec3 sd = rotmat(vec3(1.0, 1.0, 0.0), time * 0.25) * normalize(vec3(0.0, 1.0, 0.0));
    return vec4(sqrt(max(0.0, (dot(sd, VECTOR_UP)))) * mix((color / colorw) + atm * min(0.6, 0.01 * depr), atm * 0.41, min(0.99, 0.00001 * depr)), cv);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
        vec2 uv = fragCoord.xy / resolution.xy;
        UV = uv;
        vec3 sd = rotmat(vec3(1.0, 1.0, 0.0), time * 0.25) * normalize(vec3(0.0, 1.0, 0.0));
        float waterdepth = 2.1;
        vec3 wfloor = vec3(0.0, -waterdepth, 0.0);
        vec3 wceil = vec3(0.0, 0.0, 0.0);
        vec3 orig = vec3(0.0, 2.0, 0.0);
        vec3 ray = getRay(uv);
        float hihit = intersectPlane(orig, ray, wceil, vec3(0.0, 1.0, 0.0));

    Sphere sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    Sphere sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);

    if(ray.y >= -0.01){
        vec3 atm = getatm(ray);
        vec3 C = atm * 2.0 + sun(ray);

        vec3 atmorg = vec3(0,planetradius,0);
        Ray r = Ray(atmorg, ray);
        float hitfloor = rsi2(r, sphere1);
        float hitceil = rsi2(r, sphere2);
        vec4 clouds = internalmarchconservative(atm, ray * hitfloor, ray * hitceil, 1.0);
        C = mix(C, clouds.xyz, clouds.a);
        C = mix(C, getSunColorDirectly(0.0) * 0.4 * max(0.0, dot(sd, VECTOR_UP)), 1.0 - smart_inverse_dot(max(0.0, dot(ray, VECTOR_UP)), 31.0));
        //tonemapping
        C = normalize(C) * sqrt(length(C));
        fragColor = vec4( C,1.0);
        return;
    }
        float lohit = intersectPlane(orig, ray, wfloor, vec3(0.0, 1.0, 0.0));
    vec3 hipos = orig + ray * hihit;
    vec3 lopos = orig + ray * lohit;
        float dist = raymarchwater(orig, hipos, lopos, waterdepth);
    vec3 pos = orig + ray * dist;

        vec3 N = normal(pos.xz, 0.01, waterdepth);
    N = mix(N, VECTOR_UP, 0.8 * min(1.0, sqrt(dist*0.01) * 1.1));
    vec2 velocity = N.xz * (1.0 - N.y);
    vec3 R = normalize(reflect(ray, N));
    vec3 RF = normalize(refract(ray, N, 0.66));
    float fresnel = (0.04 + (1.0-0.04)*(pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));

    R.y = abs(R.y);
    vec3 reflection = getatm(R);
    vec3 atmorg = vec3(0,planetradius,0) + pos;
    Ray r = Ray(atmorg, R);
    float hitfloor = rsi2(r, sphere1);
    float hitceil = rsi2(r, sphere2);
   // vec4 clouds = internalmarchconservative(reflection, R * hitfloor, R * hitceil, 0.0);
    //vec3 C = fresnel * mix(reflection + sun(R), clouds.xyz, clouds.a * 0.3) * 2.0;

    vec3 C = fresnel * (reflection + sun(R)) * 2.0;

        float superscat = pow(max(0.0, dot(RF, sd)), 16.0) ;
#if ENABLE_SSS
    C += vec3(0.5,0.9,0.8) * superscat * getSunColorDirectly(0.0) * 81.0;
    vec3 waterSSScolor =  vec3(0.01, 0.33, 0.55)*  0.171 ;
    C += waterSSScolor * getSunColorDirectly(0.0) * (0.3 + getwaves(pos.xz)) * waterdepth * 0.3 * max(0.0, dot(sd, VECTOR_UP));
    //tonemapping
    #endif
    C = mix(C, getSunColorDirectly(0.0) * 0.4 * max(0.0, dot(sd, VECTOR_UP)), 1.0 - smart_inverse_dot(max(0.0, dot(ray, -VECTOR_UP)), 31.0));

    C = normalize(C) * sqrt(length(C));

        fragColor = vec4(C,1.0);
}
void main( void ) {

        vec4 c = vec4(1.0);
        mainImage(c,gl_FragCoord.xy);
        gl_FragColor = vec4( c);

}
