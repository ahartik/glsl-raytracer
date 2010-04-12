//#version 150
//#version 150
in vec4 ex_position;

uniform vec3 ambient;
uniform vec3 global_ambient;
uniform vec3 light_color;


uniform sampler2D colortex;

//out vec4 out_color;
const float blurSize = 3.0/1024;

//float kernel[] = float[](0.05,0.07,0.09,0.105,0.12,0.135,0.15,0.155,0.16);
//float kernel[] = float[](0.05,0.07,0.09,0.105,0.12,0.135,0.15,0.155,0.16);
//#define BIGBLUR

uniform bool bright_cut;

#ifdef BIGBLUR
float kernel[] = float[](0.05,0.07,0.09,0.105,0.12,0.135,0.15,0.155,0.16);
#else
float kernel[] = float[](0.05,0.09,0.12,0.15,0.16);
#endif
void main()
{
    vec2 tc = (ex_position.xy+vec2(1,1))*0.5+blurSize*0.5;
    vec4 sum = vec4(0.0);

    vec4 col = texture2D(colortex, vec2(tc.x, tc.y));

    #define ADD(xi,s) \
    sum += texture2D( colortex, vec2(tc.x s xi*blurSize,tc.y)) * kernel[xi]

#ifdef BIGBLUR
    ADD(8,-);
    ADD(7,-);
    ADD(6,-);
    ADD(5,-);
    ADD(5,+);
    ADD(6,+);
    ADD(7,+);
    ADD(8,+);
#endif
    ADD(4,-);
    ADD(3,-);
    ADD(2,-);
    ADD(1,-);
    ADD(0,+);
    ADD(1,+);
    ADD(2,+);
    ADD(3,+);
    ADD(4,+);

    //sum/=2;
    //gl_FragData[0] = col+sum*0.3;
    //if(dot(sum,sum)<4)sum=vec4(0,0,0,0);
    gl_FragData[0] = sum;
    //gl_FragData[0] = col+sum*0.1*dot(sum,sum);
}
