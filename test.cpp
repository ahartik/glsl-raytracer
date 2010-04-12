#include"lib/glinc.h"
#include"lib/timef.h"
#include"lib/math.hpp"
#include"State.hpp"
#include"lib/glsl.hpp"
#include"lib/glErrorUtil.h"
#include"lib/FramebufferObject.hpp"
#include"lib/VertexbufferObject.hpp"

#include"gl3.h"
#include <SDL/SDL.h>

const int WIDTH = 512;
const int HEIGHT = 512;

void InitGL(int Width, int Height)	        
{
    glViewport(0, 0, Width, Height);
    //
    
    //
    //
    //
    glClearDepth(1.0);				
    glDepthFunc(GL_LESS);			
    glEnable(GL_DEPTH_TEST);	
    glShadeModel(GL_SMOOTH);
    //glDisable(GL_DITHER);
    glDisable(GL_COLOR_MATERIAL);
    glColor4f(1,1,1,1);
}

GLuint torus_vbo;


GLuint createTexture(GLuint type = GL_RGBA32F,GLuint filter = GL_NEAREST,int downsample=0)
{
    GLuint texture;
    glGenTextures(1,&texture);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);

    glTexImage2D(GL_TEXTURE_2D,0,type,WIDTH>>downsample,HEIGHT>>downsample,0,GL_RGBA,GL_FLOAT,NULL);
#if 1
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);//GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);//GL_LINEAR);
#else 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#endif
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return texture;
}

class Test
{
    State& state;
    ProgramPtr prog_s;
    ProgramPtr prog_p;
    ProgramPtr bloom1;
    ProgramPtr bloom2;
    ProgramPtr bloom3;

    //#define UF(u,v) u(state,#u,v)
    /*
       Uniform<mat4> projection("projection");
    //UF(mat4,projection,perspective(deg_to_rad(45),aspect,0.1,100));
    UF(mat4,model_view,identity());
    UF(vec3,diffuse,vec3(0.4,0.4,0.4));
    UF(vec3,ambient,vec3(1,1,1));
    UF(vec3,specular,vec3(1,1,1));
    UF(vec3,global_ambient,vec3(0.2,0.2,0.2));
    UF(vec3,light_color,vec3(1,1,1));
    UF(vec3,light_pos,vec3(0,0,2));
    UF(float,shininess,10);
    UF(mat3,normal_matrix,mat3());
    */
    public:
    Uniform<float> exposure;
    Uniform<mat3> eye_rot;
    Uniform<vec3> eye_pos;
    private:
    Uniform<vec3> ambient;
    Uniform<vec3> global_ambient;
    Uniform<vec3> light_color;
    Uniform<vec3> light_pos;
    Uniform<int> colortex_u;
    Uniform<int> postex_u;
    Uniform<int> dirtex_u;
    Uniform<vec3> ball1;
    Uniform<vec3> ball2;
    Uniform<vec3> ball3;
      Uniform<mat4> projection;
    VertexbufferObject::Ptr vbo;

    FramebufferObject fbo;

    FramebufferObject bloomFbo;

    GLint attribute_position;
    GLint attribute_normal;

    GLuint postfx_tex1;
    GLuint postfx_tex2;
    GLuint postfx_tex3;

    GLuint postex;
    GLuint dirtex;
    GLuint colortex;

    GLuint temp_postex;
    GLuint temp_dirtex;
    GLuint temp_colortex;

    public:
    struct Vertex
    {
        vec3 position;
        vec3 normal;
    };
    bool pause;
    ShaderPtr primaryShader;
    ShaderPtr secondaryShader;


#define UF(u,v) u(#u,v)
    Test(float aspect):
        state(State::get()),
        UF(exposure,2),
        UF(eye_rot,mat3::Identity()),
        UF(eye_pos,vec3(0,0,-2)),
        UF(ambient,vec3(1,1,1)),
        UF(global_ambient,vec3(0.1,0.1,0.1)),
        UF(light_color,vec3(1,1,1)),
        UF(light_pos,vec3(10,10,-4)),
        colortex_u("colortex",0),
        postex_u("postex",1),
        dirtex_u("dirtex",2),
        ball1("ball[0]",vec3(3,3,-18)),
        ball2("ball[1]",vec3(3,3,-18)),
        ball3("ball[2]",vec3(3,3,-18)),
        UF(projection,perspective(deg_to_rad(45),aspect,0.1,100)),
        pause(0)
    {
#undef UF
        glDisable(GL_BLEND);
        prog_p = Program::create();
        prog_s = Program::create();
        bloom1 = Program::create();
        bloom2 = Program::create();
        bloom3 = Program::create();
        ShaderPtr vs = Shader::create_from_file(GL_VERTEX_SHADER,"simple.vp");
        primaryShader = Shader::create_from_file(GL_FRAGMENT_SHADER,"starttextures.fp");
        secondaryShader = Shader::create_from_file(GL_FRAGMENT_SHADER,"secondary_ray_final.fp");
        prog_p->attach(vs);
        prog_p->attach(primaryShader);
        prog_p->link();

        prog_s->attach(vs);
        prog_s->attach(secondaryShader);
        prog_s->link();

        bloom1->attach(vs);
        bloom2->attach(vs);
        bloom3->attach(vs);
        bloom1->attach(Shader::create_from_file(GL_FRAGMENT_SHADER,"bloom_vertical.fp"));
        bloom2->attach(Shader::create_from_file(GL_FRAGMENT_SHADER,"bloom_horizontal.fp"));
        bloom3->attach(Shader::create_from_file(GL_FRAGMENT_SHADER,"bloom_mix.fp"));

        bloom1->link();
        bloom2->link();
        bloom3->link();


        state.set_program(prog_p);

        attribute_position = prog_p->get_attrib_location("position");
        attribute_normal = prog_p->get_attrib_location("normal");
        std::cout<<prog_p->log()<<"\n";
        checkErrorsGL("test constructor");
        build_vbo();


        colortex = createTexture(GL_RGBA32F);
        postex = createTexture();
        dirtex = createTexture();
        GLuint tempType = GL_RGBA32F;
        temp_colortex = createTexture(tempType);
        temp_postex = createTexture(tempType);
        temp_dirtex = createTexture(tempType);
        
        postfx_tex3 = createTexture(GL_RGBA16F,GL_LINEAR,0);
        postfx_tex2 = createTexture(GL_RGBA16F,GL_LINEAR,0);
        postfx_tex1 = createTexture(GL_RGBA8,  GL_LINEAR,0);


        //glEnable(GL_ACTIVE_TEXTURE);
        /*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,colortex);
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,postex);
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,dirtex);
        glEnable(GL_TEXTURE_2D);
        */


        fbo.Bind();

        fbo.AttachTexture(GL_TEXTURE_2D,colortex,GL_COLOR_ATTACHMENT0);
        fbo.AttachTexture(GL_TEXTURE_2D,postex,GL_COLOR_ATTACHMENT1);
        fbo.AttachTexture(GL_TEXTURE_2D,dirtex,GL_COLOR_ATTACHMENT2);

        assert(fbo.IsValid(std::cout));

        /*

           GLenum bufs[] = {
           GL_COLOR_ATTACHMENT0,
           GL_COLOR_ATTACHMENT1,
           GL_COLOR_ATTACHMENT2
           };

           glDrawBuffers(3,bufs);
           */

        GLenum bufs[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
        };

        glDrawBuffers(3,bufs);
        FramebufferObject::Disable();

        //prog_p->use();

        /*
           glBindFragDataLocation(prog_p->id(),0,"color_out");
           glBindFragDataLocation(prog_p->id(),1,"pos_out");
           glBindFragDataLocation(prog_p->id(),2,"dir_out");
           */

        checkErrorsGL("test constructor 2");




        /*
        prog_s->use();
        prog_s->set_var("colortex",0);
        prog_s->set_var("postex",1);
        prog_s->set_var("dirtex",2);
*/
        checkErrorsGL("test constructor 3");

    }

    void build_vbo()
    {
        Vertex v[8];
        int i = 0;
        int ni = 0;
        v[i++].position = vec3(1,-1,-.1);
        v[i++].position = vec3(-1,-1,-.1);
        v[i++].position = vec3(-1,1,-.1);
        v[i++].position = vec3(1,1,-.1);
        v[ni++].normal = vec3(0,0,1);
        v[ni++].normal = vec3(0,0,1);
        v[ni++].normal = vec3(0,0,1);
        v[ni++].normal = vec3(0,0,1);

        VertexAttributeSpec p[2];
        p[0].offset = 0;
        p[0].size = 3;
        p[0].attrib = attribute_position;

        p[1].offset = sizeof(vec3);
        p[1].size = 3;
        p[1].attrib = attribute_normal;

        vbo = VertexbufferObject::create(p,2,v,4);
        checkErrorsGL("vbo initialized");
    }
    void draw_vbo()
    {
        glPointSize(10);
        vbo->draw(GL_QUADS);
    }
    vec3 balpos(int i)
    {
#if 1
        static float t = timef();
        if(!pause)t = timef();
        float t2=t+i*M_PI*2/3.0;
        vec4 bal = vec4(sin(t2),cos(t2),0,0);
        bal=rotate(t*1.5+3,1,0,0)*bal;
        //bal.z()=0;//sin(t+32)*cos(t-32);
        //std::swap(bal.z(),bal.y());
        return vec3(bal.x()*1,1*bal.y(),-4.0+bal.z())*1.0;
#else 
        float t = timef()*.5;
        t+=i*M_PI*2/3.0;
        return vec3(sin(t),cos(t),sin(t)*cos(t)-4);
#endif
    }
    void set_texture(int unit,int tex)
    {
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(GL_TEXTURE_2D,tex);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
    }
    void draw_scene()
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        float t = timef();
        float z = sin(t*3+32)*cos(t);
        ball1.var() = balpos(1);
        ball2.var() = balpos(2);
        ball3.var() = balpos(3);
        //vec3(1*sin(t),1*cos(t),-3+1*sin(t*4)*cos(t));
        //ball1.var()=vec3(0,0,2);

        projection.var().setIdentity();
        /*
        model_view.var().setIdentity();
        model_view.var()*=translate(0,0,-0.5);
        
        model_view.var().setIdentity();
        model_view.var()*=translate(0,0,-4.5);
        model_view.var()*=rotate(timef(),0,1,0);

*/
        //*ormal_matrix.var()=::normal_matrix(model_view.var());




        state.set_program(prog_p);
        fbo.Bind();
        //fbo.AttachTexture(GL_TEXTURE_2D,colortex,GL_COLOR_ATTACHMENT0);
        //fbo.AttachTexture(GL_TEXTURE_2D,postex,GL_COLOR_ATTACHMENT1);
        //fbo.AttachTexture(GL_TEXTURE_2D,dirtex,GL_COLOR_ATTACHMENT2);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        state.begin();
        draw_vbo();
        FramebufferObject::Disable();

        checkErrorsGL("draw 1");
        state.end();

        //secondary rays
#if 1
        state.set_program(prog_s);
        prog_s->use();

        //

#if 1
#endif
        const int depth = 3;

#define BLOOM
        
        for(int i=1;i<=depth;i++)
        {
            if(i!=depth)
            {
                fbo.Bind();
                fbo.AttachTexture(GL_TEXTURE_2D,temp_colortex,GL_COLOR_ATTACHMENT0);
                fbo.AttachTexture(GL_TEXTURE_2D,temp_postex,GL_COLOR_ATTACHMENT1);
                fbo.AttachTexture(GL_TEXTURE_2D,temp_dirtex,GL_COLOR_ATTACHMENT2);
            }else{
#ifdef BLOOM
                bloomFbo.Bind();
                bloomFbo.AttachTexture(GL_TEXTURE_2D,postfx_tex1,GL_COLOR_ATTACHMENT0);
#endif
            }
            set_texture(0,colortex);
            set_texture(1,postex);
            set_texture(2,dirtex);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glClear(GL_COLOR_BUFFER_BIT);
            glColor4f(1,1,1,1);

            state.begin();

            prog_s->set_var("colortex",0);
            prog_s->set_var("postex",1);
            prog_s->set_var("dirtex",2);
            prog_s->set_var("finalout",0);//depth==i);

            checkErrorsGL("sbegin 2");
            draw_vbo();
            checkErrorsGL("sdraw 2");
            state.end();
            checkErrorsGL("sdraw 2.1");

            if(i!=depth)
            {
                FramebufferObject::Disable();
                std::swap(temp_postex,postex);
                std::swap(temp_colortex,colortex);
                std::swap(temp_dirtex,dirtex);
            }else
            {
#ifdef BLOOM
                FramebufferObject::Disable();
#endif
            }
        }

#ifdef BLOOM

        const int blurRounds = 5;
        for(int i=1;i<=blurRounds;i++)
        {
            //BLOOOM / blur
            state.set_program(bloom1);

            {
                bloomFbo.Bind();
                //bloomFbo.AttachTexture(GL_TEXTURE_2D,postfx_tex1,GL_COLOR_ATTACHMENT0);
                bloomFbo.AttachTexture(GL_TEXTURE_2D,postfx_tex3,GL_COLOR_ATTACHMENT0);
            }

            if(i==1)
                set_texture(0,postfx_tex1);
            else 
                set_texture(0,postfx_tex2);
            state.begin();
            bloom1->set_var("colortex",0);
            draw_vbo(); // vertical draw
            state.end();

            {
                std::swap(postfx_tex2,postfx_tex3);
                bloomFbo.AttachTexture(GL_TEXTURE_2D,postfx_tex3,GL_COLOR_ATTACHMENT0);
            }

            state.set_program(bloom2);
            set_texture(0,postfx_tex2);
            state.begin();
            bloom2->set_var("colortex",0);
            draw_vbo();
            state.end();
            std::swap(postfx_tex2,postfx_tex3);
        }
        checkErrorsGL("blur");

        FramebufferObject::Disable();

        state.set_program(bloom3);
        set_texture(0,postfx_tex1);
        set_texture(1,postfx_tex2);
        state.begin();
        bloom3->set_var("colortex",0);
        bloom3->set_var("blurtex",1);
        draw_vbo();
        state.end();
        
#endif

#endif
#if 0
        //disable multitexturing 
        glActiveTexture(GL_TEXTURE1);
        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE2);
        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);

        
        glUseProgram(0);
    
        glActiveTexture(GL_TEXTURE0);

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D,colortex);
        glBegin(GL_QUADS);
        {
            glNormal3f(0,0,1);
            glTexCoord2f(0, 0.5); glVertex3f(-1, 0, -0.5f);
            glTexCoord2f(0.5, 0.5); glVertex3f( 0, 0, -0.5f);
            glTexCoord2f(0.5, 1.0); glVertex3f( 0,  1, -0.5f);
            glTexCoord2f(0, 1.0); glVertex3f(-1,  1, -0.5f);
        }
        glEnd();

        glBindTexture(GL_TEXTURE_2D,postex);
        glBegin(GL_QUADS);
        {
            glNormal3f(0,0,1);
            glTexCoord2f(0.5, 0.5); glVertex3f(0, 0, -0.5f);
            glTexCoord2f(1, 0.5); glVertex3f( 1, 0, -0.5f);
            glTexCoord2f(1, 1.0); glVertex3f( 1,  1, -0.5f);
            glTexCoord2f(0.5, 1.0); glVertex3f(0,  1, -0.5f);
        }
        glEnd();

        glBindTexture(GL_TEXTURE_2D,dirtex);
        glBegin(GL_QUADS);
        {
            glNormal3f(0,0,1);
            glTexCoord2f(0.5, 0); glVertex3f(0, -1, -0.5f);
            glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.5f);
            glTexCoord2f(1, 0.5); glVertex3f( 1,  0, -0.5f);
            glTexCoord2f(0.5, 0.5); glVertex3f(0,  0, -0.5f);
        }
        glEnd();
#endif
        assert(fbo.IsValid(std::cout));
        /*
           state.set_program(prog_s);
           prog_s->use();
           prog_s->set_var("colortex",0);
           prog_s->set_var("postex",1);
           prog_s->set_var("dirtex",2);


           state.begin();
           checkErrorsGL("begin 2");
           std::cout<<"f\n";
           draw_vbo();
           checkErrorsGL("draw 2");
           state.end();
           */

    }
    void set_aspect_ratio(float aspect)
    {
        //projection.var() = perspective(deg_to_rad(45),aspect,0.1,100);
    }

};




int main(int argc, char **argv) 
{  
    int done;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    int w = WIDTH;
    int h = HEIGHT;
    if ( SDL_SetVideoMode(w, h, 0, SDL_OPENGL|SDL_RESIZABLE) == NULL ) {
        fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }
    system("cpp primary_ray.fp|grep -v \"#\" > primary_ray_final.fp");
    system("cpp secondary_ray.fp|grep -v \"#\" > secondary_ray_final.fp");
    glewInit();
    glutInit(&argc,argv);

    InitGL(w,h);

    Test test(w/double(h));

    done = 0;
    int framecount = 0;
    double t = timef();
    double lt = timef();
    double xrot=0;
    double yrot=0;
    bool spaceheld = 0;
    while ( ! done ) {
        framecount++;
        double dt = timef()-lt;
        lt = timef();
        if(framecount>100)
        {
            double ft = timef();
            std::cout<<"fps="<<framecount/(ft-t)<<"\n";
            framecount = 0;
            t = ft;
        }
        test.draw_scene();
        SDL_GL_SwapBuffers();

        Uint8* keys = SDL_GetKeyState(NULL);
        mat4 rot = rotate(xrot,0,1,0)*rotate(yrot,1,0,0);
        mat3 rot3 = rot.corner(Eigen::TopLeft,3,3);
        test.eye_rot.var() = rot.corner(Eigen::TopLeft,3,3);

        if(keys[SDLK_s])
            test.eye_pos.var()+=rot3*vec3(0,0,2)*dt;
        if(keys[SDLK_w])
            test.eye_pos.var()+=rot3*vec3(0,0,-2)*dt;
        if(keys[SDLK_a])
            test.eye_pos.var()+=rot3*vec3(-2,0,0)*dt;
        if(keys[SDLK_d])
            test.eye_pos.var()+=rot3*vec3(2,0,0)*dt;

        if(keys[SDLK_UP])
            yrot+=dt;
        if(keys[SDLK_DOWN])
            yrot-=dt;
        if(keys[SDLK_LEFT])
            xrot+=dt;
        if(keys[SDLK_RIGHT])
            xrot-=dt;

        if(keys[SDLK_SPACE]){
        } else {
        }
            
        if(keys[SDLK_PLUS])
        {
            float& e = test.exposure.var();
            e+=dt;
            std::cout<<"exposure = "<<e<<"\n";
        }
        if(keys[SDLK_MINUS])
        {
            float& e = test.exposure.var();
            e-=dt;
            std::cout<<"exposure = "<<e<<"\n";
        }
        /**/
        { SDL_Event event;
            while ( SDL_PollEvent(&event) ) {
                switch(event.type){
                    case SDL_QUIT : 
                        done = 1;
                        break;
                    case SDL_KEYDOWN: 
                        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                            done = 1;
                        }
                        if ( event.key.keysym.sym == SDLK_SPACE){
                            test.pause^=1;
                            std::cout<<"pause = "<<test.pause<<"\n";
                        }
                        break;
                    case SDL_VIDEORESIZE:
                        /*
                           SDL_SetVideoMode(event.resize.w,event.resize.h,0,SDL_OPENGL|SDL_RESIZABLE);
                           InitGL(event.resize.w,event.resize.h);    

                           test.set_aspect_ratio(event.resize.w/(float(event.resize.h)));
                           */
                        break;
                }
            };

        }

    }
    SDL_Quit();
    return 1;
}
