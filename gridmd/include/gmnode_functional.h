#ifndef GMNODE_FUNCTIONAL_H
#define GMNODE_FUNCTIONAL_H
#include "gmnode.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

class gmNodeActionFunctional : public gmNodeAction {
public:
    typedef boost::function< int (gmNodeAction*) > t_function;
    gmNodeActionFunctional(t_function        function) : m_function(function) {}
    int OnExecute() { return m_function(this); }
    gmNodeActionFunctional* Clone() const { return new gmNodeActionFunctional(m_function); }
private:
    t_function m_function;
};

class gmNodeActionVoidFn : public gmNodeAction {
public:
    typedef boost::function< int (void) > t_function;
    gmNodeActionVoidFn(t_function        function) : m_function(function) {}
    int OnExecute() { return m_function(); }
    gmNodeActionVoidFn* Clone() const { return new gmNodeActionVoidFn(m_function); }
private:
    t_function m_function;
};


#endif // GMNODE_FUNCTIONAL_H
