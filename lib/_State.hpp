#ifndef STATE_HPP
#define STATE_HPP

#include"glsl.hpp"
#include"math.hpp"
#include<stack>

class State;
class StateVarBase
{
  State& owner;
  protected:
  State& get_owner()
  {
    return owner;
  }
  public:
  StateVarBase(State&);
  virtual void begin()=0;
  virtual void end()
  {
  }
  virtual ~StateVarBase()
  {
  }
};

class State
{
  private:
    std::vector<StateVarBase*> vars;
    ProgramPtr program;
  public:
    State(ProgramPtr p):program(p)
  {
  }
    State()
    {
    }
    void set_program(ProgramPtr p)
    {
      program = p;
    }
    ProgramPtr get_program()
    {
      return program;
    }
    void add_var(StateVarBase* v)
    {
      vars.push_back(v);
    }
    void begin()
    {
      if(program==0)throw std::logic_error("State::begin(): no shader program");
      program->use();
      for(int i=0;i<vars.size();i++)
        vars[i]->begin();
    }
    void end()
    {
      for(int i=0;i<vars.size();i++)
        vars[i]->end();
    }
};


StateVarBase::StateVarBase(State& o):owner(o)
{
  o.add_var(this);
}

template<typename T>
class StackVar:public StateVarBase
{
  public:
  private:
    std::string m_name;
    std::stack<T>  m_var_stack;
    StackVar(const StackVar& a);
  public:

    StackVar(State& o,const std::string& s):StateVarBase(o),m_name(s)
  {
  }

    T& var()
    {
      return m_var_stack.top();
    }

    const std::string& name()
    {
      return m_name;
    }
    void push(T t)
    {
      m_var_stack.push(t);
    }
    void push()
    {
      push(var());
    }
    void pop()
    {
      m_var_stack.pop();
    }

    ~StackVar()
    {
    }
};

template<typename T>
class Uniform:public StackVar<T>
{
  private:
    int loc;
  public:
    Uniform(State& o,const std::string& s):StackVar<T>(o,s)
  {
    loc = -2;
  }
    Uniform(State& o,const std::string& s,const T& t):StackVar<T>(o,s)
  {
    push(t);
    loc = -2;
  }
    void begin()
    {
      if(loc == -2){
        loc = this->get_owner().get_program()->get_uniform_location(this->name());
        std::cout<<this->name()<<"\n";
      }
      this->get_owner().get_program()->set_var(loc,this->var());
    }
    void end()
    {
    }
};

#endif
