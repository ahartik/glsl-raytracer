//#version 150
//#version 150
in vec4 ex_position;
in vec3 ex_eyedir;
in vec3 ex_light_pos;

uniform vec3 ambient;
uniform vec3 global_ambient;
uniform vec3 light_color;

uniform mat3 eye_rot;
uniform vec3 eye_pos;
//out vec4 out_color;

void main()
{
    vec3 P = vec3(ex_position.xy,-2);
    vec3 d = normalize(P);
    d = eye_rot*d;
    P = eye_pos;
    gl_FragData[0].rgb =vec3(0,0,0);
    gl_FragData[0].a = 1;
    gl_FragData[1]=vec4(P+d*0.01,1);
    gl_FragData[2]=vec4(d,1);
}
