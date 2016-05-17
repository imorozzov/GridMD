#ifndef GMREDIRECTOR_H
#define GMREDIRECTOR_H

#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <typeinfo>
#include <map>
#include <wx/thread.h>


/** \file gmredirector.h
    \en \brief Classes that implements logic to support one unique copy of
               arbitary type object for each \ref gmTask execution using \ref gmThreadPool.

**/


# define USING_GMTHREADS 1

class gmThreadPool;


///\en Class used to abstract from the specialization of \ref gmRedirector based classes.
class gmRedirectorBase {
public:

    ///\en Method to check the specialization of derived template classes.
    ///    It can be useful to distinguish various \ref gmRedirector based classes specializations.
    ///    \return std::type_info object of derived template class specialized type.
    virtual const std::type_info& ObjectTypeInfo() const = 0;

    virtual void RemoveObject(gmThreadId idToDelete = wxThread::GetCurrentId()) = 0;

protected:
    gmRedirectorBase(){;}
    virtual ~gmRedirectorBase(){;}

    mutable wxMutex mMutex;

    friend class gmThreadPool;
};

///\en Class used to store one unique instance of specialized type for
///    each of the individual \ref gmTask executions by the \ref gmThreadPool.
template <typename T>
class gmRedirector : public gmRedirectorBase
{
public:

    ///\en Removes object mapped with \a idToDelete thread index.
    ///    By default, removes object mapped with a \a caller thread.
    ///\param idToDelete Thread index to delete object that is mapped with.
    virtual void RemoveObject(gmThreadId idToDelete = wxThread::GetCurrentId()) {
        wxMutexLocker lock(mMutex);
        typename std::map<gmThreadId, T*>::iterator iter = mObjectsMap.find(idToDelete);
        if (iter != mObjectsMap.end())
            delete iter->second;
        mObjectsMap.erase(idToDelete);
    }


    ///\en Gets object mapped with a \a caller thread.
    ///\return Pointer to object mapped with a \a caller thread.
    T* GetObject() const {
        wxMutexLocker lock(mMutex);
        if (wxThread::IsMain())
            return GetMainObject();
        else
            return MakeObject();
    }

    virtual const std::type_info& ObjectTypeInfo() const { return typeid(T); }

protected:

    gmRedirector(){;}
    virtual ~gmRedirector() {
        typename std::map<gmThreadId, T*>::iterator mapIter = mObjectsMap.begin();
        for(; mapIter != mObjectsMap.end(); ++mapIter) {
            delete mapIter->second;
        }
    }

    virtual T* GetMainObject() const = 0;
    virtual T* MakeObject() const = 0;

    T* FindObject() const {
        typename std::map<gmThreadId, T*>::const_iterator iter = mObjectsMap.find(wxThread::GetCurrentId());
        if (iter != mObjectsMap.end())
            return iter->second;
        else
            return NULL;
    }
    mutable std::map<gmThreadId, T*> mObjectsMap;

    friend class gmThreadPool;
};


///\en Class to store a prototype to get it's unique copy
/// for each of executed \ref gmTask in \ref gmThreadPool.
template <typename T>
class gmRedirectorPrototyped : public gmRedirector<T>
{
//protected: //?
public:
    gmRedirectorPrototyped(const T& prototype) :
        mPrototype(new T(prototype)),
        mMainThreadObject(new T(prototype))
    {;}

    virtual ~gmRedirectorPrototyped() {
        delete mPrototype;
        delete mMainThreadObject;
    }

    virtual T* GetMainObject() const { return mMainThreadObject; }
    virtual T* MakeObject() const {
        T* object  = this->FindObject();
        if (!object) {
            object = new T(*mPrototype);
            this->mObjectsMap[wxThread::GetCurrentId()] = object;
        }
        return object;
    }
    T* mPrototype;
    T* mMainThreadObject;

    friend class gmThreadPool;
};


#endif // GMREDIRECTOR_H
