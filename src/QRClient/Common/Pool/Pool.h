#ifndef __POOL_H__
#define __POOL_H__

template<class T>
class TPool : public CPoolBase
{
public:
    TPool<T>() {};
    TPool<T>(int nInit)
    {
        CPoolBase::CPoolBase(nInit);
    };
    virtual ~TPool()
    {
        ClearPool();
    };

    virtual T*      Pop()
    {
        return (T *)CPoolBase::PopOrg();
    }
    virtual void    Push(T *tp)
    {
        CPoolBase::PushOrg(tp);
    }

protected:
    virtual LPVOID  NewFunc()
    {
        return (LPVOID)(new T);
    }
    virtual void    DeleteFunc(LPVOID tp)
    {
        delete(T *)tp;
        tp = NULL;
    }
};

#endif  // __POOL_H__
