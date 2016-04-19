#ifndef GRIDMD_REDIRECTOR_H
#define GRIDMD_REDIRECTOR_H

#include <typeinfo>
#include <logexc.h>

class gmdRedirectorBase {
public:
    virtual const std::type_info& ObjectTypeInfo() const = 0;
    virtual void RemoveObject() = 0;

protected:
    gmdRedirectorBase(){;}
    virtual ~gmdRedirectorBase(){;}

    friend class gmdThreadPool;
};


template <typename T>
class gmdRedirector : public gmdRedirectorBase
{
public:

    virtual void RemoveObject() {
        LOGMSG(vblOERR, "template <typename T> void gmdRedirector::RemoveObject() "
                        "is not implemented", false);
    }

    T* GetObject() const {
        LOGMSG(vblOERR, "template <typename T> T* gmdRedirector::GetObject() const "
                        "is not implemented", false);
        return NULL;
    }


    virtual const std::type_info& ObjectTypeInfo() const { return typeid(T); }

protected:

    gmdRedirector(){;}
    virtual ~gmdRedirector() {;}

    friend class gmdThreadPool;
};



template <typename T>
class gmdRedirectorPrototyped : public gmdRedirector<T>
{
protected:
    gmdRedirectorPrototyped(const T& prototype)
    {;}

    virtual ~gmdRedirectorPrototyped() {;}
    friend class gmdThreadPool;
};

#endif // GRIDMD_REDIRECTOR_H
