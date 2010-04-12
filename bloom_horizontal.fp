//#version 150
//#version 150
in vec4 ex_position;

uniform vec3 ambient;
uniform vec3 global_ambient;
uniform vec3 light_color;


//uniform sampler2D blurtex;
uniform sampler2D colortex;

uniform float exposure;

//out vec4 out_color;
const float blurSize =3.0/1024;

//#define BIGBLUR

//float kernel[] = float[](0.01,0.02,0.03,0.04,0.05,0.05,0.06,0.07,0.08);

#ifdef BIGBLUR
float kernel[] = float[](0.05,0.07,0.09,0.105,0.12,0.135,0.15,0.155,0.16);
#else
float kernel[] = float[](0.05,0.09,0.12,0.15,0.16);
#endif
void main()
{
    vec2 tc = (ex_position.xy+vec2(1,1))*0.5+blurSize*0.5;
    vec4 sum = vec4(0.0);

    //vec4 col = texture2D(colortex, vec2(tc.x, tc.y));

    #define ADD(xi,s) \
    sum += texture2D( colortex, vec2(tc.x,tc.y s xi*blurSize)) * kernel[xi]
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
    //sum*=0.5;
    /*

    sum += texture2D( blurtex, vec2(tc.x,tc.y - 4.0*blurSize)) * kernel[4];
    sum += texture2D( blurtex, vec2(tc.x,tc.y - 3.0*blurSize)) * kernel[3];
    sum += texture2D( blurtex, vec2(tc.x,tc.y - 2.0*blurSize)) * kernel[2];
    sum += texture2D( blurtex, vec2(tc.x,tc.y - blurSize)) * kernel[1];
    sum += texture2D( blurtex, vec2(tc.x,tc.y )) * kernel[0];
    sum += texture2D( blurtex, vec2(tc.x,tc.y + blurSize)) * kernel[1];
    sum += texture2D( blurtex, vec2(tc.x,tc.y + 2.0*blurSize)) * kernel[2];
    sum += texture2D( blurtex, vec2(tc.x,tc.y + 3.0*blurSize)) * kernel[3];
    sum += texture2D( blurtex, vec2(tc.x,tc.y + 4.0*blurSize)) * kernel[4];

    */

    //sum = vec4(1,1,1,1)-exp(-exposure*sum);
    gl_FragData[0] =sum;

    //gl_FragData[0] = sum;
    //gl_FragData[0] = col+sum*0.1*dot(sum,sum);
}
