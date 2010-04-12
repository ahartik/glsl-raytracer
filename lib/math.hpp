#ifndef MATRICES_HPP
#define MATRICES_HPP

#include<eigen2/Eigen/Core>
#include<eigen2/Eigen/LU>
#include<cmath>

typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;
typedef Eigen::Matrix2f mat2;

typedef Eigen::Vector3f vec3;
typedef Eigen::Vector2f vec2;
typedef Eigen::Vector4f vec4;

typedef Eigen::Vector3i ivec3;
typedef Eigen::Vector2i ivec2;
typedef Eigen::Vector4i ivec4;


float deg_to_rad(float f)
{
  return (M_PI/180)*f;
}
float rad_to_deg(float f)
{
  return (180/M_PI)*f;
}

Eigen::Matrix4f translate(float x,float y,float z)
{
  float vals[]= 
  {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    x,y,z,1
  };
  return Eigen::Matrix4f(vals);
}
Eigen::Matrix4f translate(const Eigen::Vector3f& v)
{
  return translate(v.x(),v.y(),v.z());
}
Eigen::Matrix4f rotate(float angle,Eigen::Vector3f v)
{
  float c = cosf(angle);
  float s = sinf(angle);
  v.normalize();
  float x = v.x();
  float y = v.y();
  float z = v.z();
  const float vals[] = 
  {
    x*x*(1-c)+c  ,x*y*(1-c)-z*s,x*z*(1-c)+y*s,0,
    y*x*(1-c)+z*s,y*y*(1-c)+c  ,y*z*(1-c)-x*s,0,
    x*z*(1-c)-y*s,y*z*(1-c)+x*s,z*z*(1-c)+c  ,0,
    0            ,0            ,0            ,1
  };
  return Eigen::Matrix4f(vals).transpose();
}
Eigen::Matrix4f rotate(float angle,float x,float y,float z)
{
  return rotate(angle,Eigen::Vector3f(x,y,z));
}

Eigen::Matrix4f perspective(float fovy,float aspect,float znear,float zfar)
{
  float f = tan(0.5*(M_PI-fovy));
  const float vals[] = 
  {
    f/aspect,0,0,0,
    0,f,0,0,
    0,0,(zfar+znear)/(znear-zfar),2*zfar*znear/(znear-zfar),
    0,0,-1,0
  };
  return Eigen::Matrix4f(vals).transpose();
}

Eigen::Matrix4f scale(float x,float y,float z)
{
  const float vals[] = 
  {
    x,0,0,0,
    0,y,0,0,
    0,0,z,0,
    0,0,0,1
  };
  return Eigen::Matrix4f(vals);
}

mat4 identity()
{
  const float vals[] = 
  {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
  };
  return mat4(vals);
}

Eigen::Matrix4f scale(float f)
{
  return scale(f,f,f); 
}

Eigen::Matrix4f scale(const Eigen::Matrix3f& v)
{
  return scale(v.x(),v.y(),v.z());
}

mat3 normal_matrix(const mat4& in)
{
  mat3 ret;
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      ret(i,j)=in(i,j);
  return ret.transpose().inverse();
}


#endif
