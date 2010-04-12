
uniform vec3 ball[3];

//const vec3 ball = vec3(1,1,-8);

vec4 colors[3] = vec4[3]( // red green blue reflect 
    vec4(0.5,1,0.5,0.5), 
    vec4(1,0.3,0.3,0.2),
    vec4(0.8,0.8,0.8,0.8)
);
float shininess[] = float[](10,10,8);
float specular[] = float[](0.5,0.2,1.0);
float diffuse[] = float[](0.5,0.8,0.0);

const float radius2 = 0.25;
const float radius = sqrt(radius2);
const float invrad = 1/radius;

vec4 background_color =vec4(0,0,0,1);// vec4(0.1,0.2,0.5,1);

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
