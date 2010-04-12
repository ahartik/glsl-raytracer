#ifndef GLSL_HPP
#define GLSL_HPP
#include"glinc.h"
#include<algorithm>
#include<stdexcept>
#include<vector>
#include<fstream>
#include<string>
#include<iterator>
#include<tr1/memory>

#include"math.hpp"

enum AttribIndex
{
  ATTRIB_POSITION = 0,
  ATTRIB_NORMAL = 1,
  ATTRIB_TEXCOORD0 = 2
};

class Shader{
  private:
    GLint m_id;
    GLuint m_type;
    std::string filename;
    Shader(const Shader& p);  //noncopyable
    Shader& operator=(const Shader& p);  //noncopyable
    Shader(GLuint type)
    {
      m_id = -1;
      m_type = type;
    }
    void load_string(const char* string)
    {
      if(m_id!=-1)destroy();
      const GLchar* source[] = { (GLchar*)(string)};
      GLuint shader = glCreateShader(m_type);
      glShaderSource(shader,1,source,NULL);
      glCompileShader(shader);
      m_id = shader;
      
      int compiled;
      glGetShaderiv(m_id,GL_COMPILE_STATUS,&compiled);
      if(!compiled)
        throw std::runtime_error(log());

    }
    
    void destroy()
    {
      glDeleteShader(m_id);
      m_id = -1;
    }
  public:
    static std::tr1::shared_ptr<Shader> create_from_string(GLuint type,const std::string& s)
    {
      std::tr1::shared_ptr<Shader> ret(new Shader(type));;
      ret->load_string(s.c_str());
      return ret;
    }
    static std::tr1::shared_ptr<Shader> create_from_file(GLuint type,const std::string& filename)
    {
      std::ifstream ifs(filename.c_str());

      std::string str(
          (std::istreambuf_iterator<char>(ifs)),
          std::istreambuf_iterator<char>()
          );
      std::tr1::shared_ptr<Shader>  foo = create_from_string(type,str.c_str());
      foo->filename = filename;
      return foo;
    }

    GLint id()const
    {
      return m_id;
    }
    
    std::string log()const
    {
      int infologlength = 0;
      int maxlength;

      glGetShaderiv(m_id,GL_INFO_LOG_LENGTH,&maxlength);

      char infolog[maxlength];

      glGetShaderInfoLog(m_id, maxlength, &infologlength, infolog);

      if (infologlength > 0)
        return infolog;
      return "";
    }
    virtual ~Shader()
    {
      destroy();
    }

};

typedef std::tr1::shared_ptr<Shader> ShaderPtr;
typedef std::tr1::weak_ptr<Shader> ShaderWeakPtr;


class Program
{

  private:
    GLint m_id;
    bool linked;
    Program(const Program& p);  //noncopyable
    Program& operator=(const Program& p);  //noncopyable

    std::vector<ShaderPtr> m_shaders;

    Program()
    {
      m_id = glCreateProgram();
      linked = false;
    }; 
  public:
    static std::tr1::shared_ptr<Program> create()
    {
      std::tr1::shared_ptr<Program> ret(new Program());
      return ret;
    }
    GLint id()
    {
      return m_id;
    }
    
    void attach(ShaderPtr s)
    {
      glAttachShader(m_id,s->id());
      m_shaders.push_back(s);
      linked = false;
    }
    void detach(ShaderPtr s)
    {
      std::vector<ShaderPtr>::iterator it = std::find(m_shaders.begin(),m_shaders.end(),s);
      if(it==m_shaders.end())throw std::runtime_error("shader not attached");
      glDetachShader(m_id,s->id());
      m_shaders.erase(it);
    }
    bool in_use()
    {
      GLint i;
      glGetIntegerv(GL_CURRENT_PROGRAM,&i);
      return i==m_id;
    }

    void use()
    {
      if(!linked){
        glLinkProgram(m_id);
        GLint link_success;
        glGetProgramiv(m_id,GL_LINK_STATUS,&link_success);
        if(link_success==GL_FALSE)
        {
          throw std::runtime_error(("Error linking:\n"+log()).c_str());
        }
        linked=1;
      }
      glUseProgram(m_id);
    }

    void link()
    {
        glLinkProgram(m_id);
        GLint link_success;
        glGetProgramiv(m_id,GL_LINK_STATUS,&link_success);
        if(link_success==GL_FALSE)
        {
          GLuint err;
          err = glGetError();
          std::string msg = ("Error linking:\n"+log()).c_str();
          if(err!=0)
            msg+=(const char*)gluErrorString(err);
          throw std::runtime_error(msg);
        }
        linked=1;
    }
    std::string log()
    {
      int infologlength = 0;
      int maxlength;

      glGetProgramiv(m_id,GL_INFO_LOG_LENGTH,&maxlength);

      char infolog[maxlength];

      glGetProgramInfoLog(m_id, maxlength, &infologlength, infolog);
      return infolog;
    }
    ~Program()
    {
      glDeleteProgram(m_id);
      m_id = -1;
    }

    int get_uniform_location(const std::string& var)
    {
      int s = glGetUniformLocation(m_id,var.c_str());
      return s;
    }

    int get_attrib_location(const std::string& att)
    {
      int s = glGetAttribLocation(m_id,att.c_str());
      return s;
    }
    void bind_attrib_location(const std::string& att,GLint loc)
    {
      glBindAttribLocation(m_id,loc,att.c_str());
    }


    template<typename T>
      void set_var(const std::string& varname,T t)
      {
        int s = get_uniform_location(varname);
        set_var(s,t);
      }

    void set_var(const int & s,int i)
    {
      glUniform1i(s,i);
    }
    void set_var(const int & s,unsigned int i)
    {
      glUniform1i(s,i);
    }
    void set_var(const int & s,float i)
    {
      glUniform1f(s,i);
    }
    void set_var(const int & s,double i)
    {
      glUniform1f(s,i);
    }

    void set_var(const int& s,const ivec2& v)
    {
      glUniform2i(s,v.x(),v.y());
    }
    void set_var(const int& s,const ivec3& v)
    {
      glUniform3i(s,v.x(),v.y(),v.z());
    }
    void set_var(const int& s,const ivec4& v)
    {
      glUniform4i(s,v.x(),v.y(),v.z(),v.w());
    }

    void set_var(const int& s,const vec3& v)
    {
      glUniform3f(s,v.x(),v.y(),v.z());
    }
    void set_var(const int& s,const vec2& v)
    {
      glUniform2f(s,v.x(),v.y());
    }
    void set_var(const int& s,const vec4& v)
    {
      glUniform4f(s,v.x(),v.y(),v.z(),v.w());
    }

    void set_var(const int& s,const Eigen::Matrix4f& m)
    {
      glUniformMatrix4fv(s,1,GL_FALSE,(const GLfloat*)(&m));
    }
    void set_var(const int& s,const Eigen::Matrix3f& m)
    {
      glUniformMatrix3fv(s,1,GL_FALSE,(const GLfloat*)(&m));
    }
};
typedef std::tr1::shared_ptr<Program> ProgramPtr;
typedef std::tr1::weak_ptr<Program> ProgramWeakPtr;


/*
template<typename T>
class Uniform
{
  public:
  private:
    ProgramWeakPtr program;
    T val;
    std::string name;
    int loc;
  public:
    Uniform(ProgramPtr p,const std::string& s)
    {
      program = p;
      name = s;
      loc = p->get_uniform_location(s);
    }
    void set(const T& v)
    {
      val = v;
      if(program.expired())throw std::logic_error("Program is out of reach when uniform "+name+" is modified");
      program.lock()->set_var(loc,val);
    }
    const T& get()
    {
      return val;
    }
    
};
*/
#endif
