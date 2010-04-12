//#version 150
//#version 150
in vec4 ex_position;
in vec3 ex_eyedir;
in vec3 ex_light_pos;

uniform sampler2D colortex;
uniform sampler2D postex;
uniform sampler2D dirtex;

uniform vec3 ambient;
uniform vec3 global_ambient;
uniform vec3 light_color;

uniform bool finalout;

//out vec4 out_color;
#include"ray_common.fp"

void main()
{
    vec2 tc = (ex_position.xy+vec2(1,1))*0.5;
    vec4 oldcolor = texture2D(colortex,tc);
    vec3 P = texture2D(postex,tc).xyz;
    vec3 P0 = P;
    vec3 d = texture2D(dirtex,tc).xyz;
    //d.z*=-1;
    
    /*
    gl_FragData[0].xyz=
        vec3(0.1,0,0)+
        0.3*normalize(P).xyz+
        0.3*normalize(oldcolor).xyz+
        abs(0.2*vec3(sin(tc.x*100),cos(tc.y*100),0))+
        abs(0.3*normalize(d).xyz);
        */
    //abs(oldcolor.xyz);//oldcolor.xyz;
#if 1

    //vec3 P = vec3(ex_position.xy,-2);

#if 0
    gl_FragData[0]=vec4(1,1,0,1);
    return;
#else

    if(P.z==0.0){
        gl_FragData[0]=oldcolor;//BACKGROUND
        gl_FragData[1]=vec4(0,0,0,0);
        gl_FragData[2]=vec4(0,0,0,0);
        return;
    }

    bool found=false;
    bool shadow =false;
    vec3 N;
    vec4 col;
    int ball_i;
    traceballs(d,P,N,col,found,ball_i,shadow);
    //vec3 d = normalize(P);
    //*/

    if(!found){
        //discard;
        gl_FragData[0]=oldcolor+oldcolor.a*background_color;
        gl_FragData[1]=vec4(0,0,0,0);
        gl_FragData[2]=vec4(0,1,0,0);
        //gl_FragData[0]=vec4(P,1);
        //gl_FragData[0]=vec4(P,1);
        return;
    }
    //gl_FragColor=oldcolor+vec4(0.1,0.1,0.1,0);

#if 1

    //vec3 N = normalize(ex_normal);

    //ambient
    vec3 A = global_ambient*ambient;
    //diffuse
    vec3 D=vec3(0,0,0),S=vec3(0,0,0);
    if(!shadow){
        vec3 L = normalize(ex_light_pos-P);
        float diff_light = max(dot(L,N),0.0);
        D = diffuse[ball_i]*light_color*diff_light;

        //specular
        vec3 ldir = normalize(ex_light_pos-P0);
        vec3 V = normalize(ldir-P);
        vec3 H = normalize(V+L);
        float specularLight = pow(max(dot(H,N),0.0),shininess[ball_i]);
        S = specular[ball_i]*light_color*specularLight;
    }

    col*=oldcolor.a;
    gl_FragData[0].rgb =oldcolor.rgb+(col.rgb)*(A+D+S);// A+S+D;
    gl_FragData[0].a = col.a*oldcolor.a;//*col;

    d = reflect(d,N);
    gl_FragData[1].xyz = P+d*0.01;
    gl_FragData[2].xyz = d;
#endif

#endif
#endif
}
