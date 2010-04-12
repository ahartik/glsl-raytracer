#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include<iterator>
#include<algorithm>
#include<vector>
#include<tr1/memory>
#include"glinc.h"
#include"glsl.hpp"

class VertexAttributeSpec
{
  public:
  int offset;

  GLenum type;
  GLboolean normalize;
  GLint attrib;
  GLint size;
  VertexAttributeSpec()
  {
    offset = 0;
    type = GL_FLOAT;
    normalize = false;
  };
};




class VertexbufferObject
{
  std::vector<VertexAttributeSpec> layout;
  GLuint stride;
  GLuint buffer;
  size_t m_size;
  VertexbufferObject()
  {
    glGenBuffers(1,&buffer);
  }
  public:
  typedef std::tr1::shared_ptr<VertexbufferObject> Ptr;
  typedef std::tr1::weak_ptr<VertexbufferObject> WeakPtr;
  GLuint id()
  {
    return buffer;
  }

  template<typename T>
  static Ptr create(VertexAttributeSpec* vas,int acount,T* data,size_t size)
  {
    Ptr ret(new VertexbufferObject());
    ret->layout.resize(acount);
    std::copy(vas,vas+acount,ret->layout.begin());
    ret->stride = sizeof(T);
    glBindBuffer(GL_ARRAY_BUFFER,ret->id());
    glBufferData(GL_ARRAY_BUFFER,sizeof(T)*size,reinterpret_cast<char*>(data),GL_STATIC_DRAW);
    ret->m_size = size;
    return ret;
  }

  size_t size()const
  {
      return m_size;
  }
  void draw(GLenum draw_mode)
  {
      draw(draw_mode,0,m_size);
  }
  void draw(GLenum draw_mode,int start,size_t count)
  {
    glBindBuffer(GL_ARRAY_BUFFER,id());
    for(size_t i = 0;i!=layout.size();++i)
    {
      glEnableVertexAttribArray(layout[i].attrib);
      glVertexAttribPointer(
          layout[i].attrib,
          layout[i].size,
          layout[i].type,
          layout[i].normalize,
          stride,
          (char*)(NULL)+layout[i].offset
          );
    }
    glDrawArrays(draw_mode,start,count);
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
};



#endif
