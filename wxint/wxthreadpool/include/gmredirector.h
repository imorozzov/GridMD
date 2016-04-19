#ifndef GMREDIRECTOR_H
#define GMREDIRECTOR_H

#include <gmthreaddefs.h>
#include <gmd/string.h>
#include <typeinfo>
#include <map>
#include <wx/thread.h>

class gmThreadPool;

class gmRedirectorBase {
public:
    virtual const std::type_info& ObjectTypeInfo() const = 0;
    virtual void RemoveObject(gmThreadId idToDelete = wxThread::GetCurrentId()) = 0;

protected:
    gmRedirectorBase(){;}
    virtual ~gmRedirectorBase(){;}

    mutable wxMutex mMutex;

    friend class gmThreadPool;
};

template <typename T>
class gmRedirector : public gmRedirectorBase
{
public:
    virtual void RemoveObject(gmThreadId idToDelete = wxThread::GetCurrentId()) {
        wxMutexLocker lock(mMutex);
        typename std::map<gmThreadId, T*>::iterator iter = mObjectsMap.find(idToDelete);
        if (iter != mObjectsMap.end())
            delete iter->second;
        mObjectsMap.erase(idToDelete);
    }

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

template <typename T>
class gmRedirectorPrototyped : public gmRedirector<T>
{
protected:
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
