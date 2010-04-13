//#version 150
//#version 150
in vec4 ex_position;

uniform vec3 ambient;
uniform vec3 global_ambient;
uniform vec3 light_color;


uniform sampler2D blurtex;
uniform sampler2D colortex;

uniform float exposure;

//out vec4 out_color;
void main()
{
    vec2 tc = (ex_position.xy+vec2(1,1))*0.5;

    vec4 sum = texture2D(blurtex,tc);
    vec4 col = texture2D(colortex,tc);

    //sum =  vec4(1,1,1,1)-exp(-0.5*exposure*sum);
    //sum = max(vec4(0,0,0,0),sum-vec4(1,1,1,1));
    //sum = vec4(1,1,1,1)-exp(-0.5*exposure*sum);
    col =  col+sum*sum;
    col =  vec4(1,1,1,1)-exp(-exposure*col);
    gl_FragColor = col;
    //gl_FragColor = col+sum;
    //gl_FragColor = sum;


    //gl_FragData[0] = sum;
    //gl_FragData[0] = col+sum*0.1*dot(sum,sum);
}
