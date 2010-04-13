
uniform vec3 ball[3];

//const vec3 ball = vec3(1,1,-8);

vec4 colors[] = vec4[]( // red green blue reflect 
    vec4(0.5,1,0.5,0.5), 
    vec4(1,0.3,0.3,0.2),
    vec4(0.8,0.8,0.8,0.8),
    vec4(1,1,1,1)

);
uniform float time;
float shininess[] = float[](10,10,8);
float specular[] = float[](0.5,0.2,1.0);
float diffuse[] = float[](0.5,0.8,0.0);

const float radius2 = 0.25;
const float radius = sqrt(radius2);
const float invrad = 1/radius;

in vec3 ex_normal8;

vec4 background_color =vec4(0,0,0,1);// vec4(0.1,0.2,0.5,1);
//vec4 background_color = vec4(0.1,0.2,0.5,1);
float func(vec3 v){
    vec2 mov;
    mov = vec2(0.0,0.0);
    //vec2 mov = vec2(sin(v.x*2*(0.5+cos(time*0.5))+time*3),cos(v.x*1*(1.0+sin(time*2))));
    mov = vec2(cos(v.x*2.4+1)*cos(time*1.3+3),sin(v.x*2)*cos(time));
    mov+=vec2(cos(v.y*2)*cos(time*1.2+2+v.x),sin(v.z*2));
    //mov*=min(1,2-3*sin(time+v.x));
    vec2 tube = vec2(-10,0)+mov*1;
    vec2 d = v.zy-tube.xy;
    //return length(d)-1;
    return length(d)-1.0;
}

vec3 funcnormal(vec3 x)
{
    float h =0.001;
#define f func
    return -normalize(
                vec3(
                f(x)-f(x+h*vec3(1,0,0)),
                f(x)-f(x+h*vec3(0,1,0)),
                f(x)-f(x+h*vec3(0,0,1))
                )
            );
#undef f 
}

void traceballs(in vec3 d,inout vec3 P,out vec3 N,out vec4 col,out bool found,out int balli,out bool shadow)
{
#if 1
    
    for(int i=0;i<800;i++)
    {
        float v = func(P);
        if(abs(v)<=0.021){
            found=true;
            N = funcnormal(P);
            break;
        }
        if(v>10)break;
        P+=d*(0.2*v);
    }

    
#else 
    //secant method
    float xn_1 = 0;
    float xn = 0.5;
    float fn,fn_1;
    fn_1 = func(P+xn_1*d);
    for(int i=0;i<40;i++){
        fn = func(P+xn*d);
        float d = (xn - xn_1) / (fn - fn_1) * fn;
        if (abs(d) < 0.1){
            found=true;
            P = P+xn*d;
            N = funcnormal(P);
            break;
        }
        xn_1 = xn;
        xn = xn - d;
        fn_1 = fn;
    }
#endif

    balli = 2;
    col = colors[balli];

    shadow = false;
    balli = 2;
}

#if 0
void traceballs(in vec3 d,inout vec3 P,out vec3 N,out vec4 col,out bool found,out int balli,out bool shadow)
{
    vec3 P0 = P;
    int bi=0;
    float mt = 100;
#define BALLRAY(i) {\
        bool hit = true;\
        vec3 o = P0-ball[i];\
        float os = o.x*o.x+o.y*o.y+o.z*o.z;\
        float b = 2*dot(d,o);\
        if(b>=0) hit=false;\
        float c = os-radius2;\
        float discr = b*b-4*c;\
        if(discr<0.01) hit=false;\
        float t0 = (-b+sqrt(discr))/2;\
        float t1 = c/t0;\
        float t = t1<0?t0:t1; \
        if(t>mt)hit=false;\
        if(t<0.010)\
            hit=false;\
        if(hit){\
            mt = min(mt,t);\
            P = P0+t*d;\
            N = (o+t*d);\
            col = colors[i];\
            found=true;\
            bi=i;\
        }\
        }
#define BALLRAY_shadow(i) {\
        bool hit = true;\
        vec3 o = P0-ball[i];\
        float os = o.x*o.x+o.y*o.y+o.z*o.z;\
        float b = 2*dot(d,o);\
        if(b>=0) hit=false;\
        float c = os-radius2;\
        float discr = b*b-4*c;\
        if(discr<0.01) hit=false;\
        float t0 = (-b+sqrt(discr))/2;\
        float t1 = c/t0;\
        float t = t1<0?t0:t1; \
        if(t<0.010)\
            hit=false;\
        if(hit){\
            shadow=true;\
        }\
        }
        
    for(int i=0;i<3;i++){
        BALLRAY(i);
    }
    if(found){
        shadow = false;
        balli = bi;
        P0 = P;
        N = normalize(N);
        d = normalize(ex_light_pos-P);
        P0+=0.1*d;
        //for(int i=0;i<3;i++){
            //BALLRAY_shadow(i);
        //}
    }
    //BALLRAY(1);
    //BALLRAY(2);
}
#endif
