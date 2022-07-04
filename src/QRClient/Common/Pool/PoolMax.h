#ifndef __POOLMAX_H__
#define __POOLMAX_H__

#include "Pool.h"

// 带空间限制的Pool
template<class T>
class TPoolMax : public TPool<T>
{
public:
    TPoolMax<T>()
        : TPool<T>()
    {
        m_bMaxSizeOver  = FALSE;
    }
    TPoolMax<T>(int nInit, int nMaxPoolSize = 20, int nDeleteStartSize = 10)
        : TPool<T>(nInit)
    {
        SetMaxPoolSize(nMaxPoolSize);
        SetDeleteStartSize(nDeleteStartSize);
    }
    virtual ~TPoolMax() {   }

protected:
    BOOL    m_bMaxSizeOver;                 // 是否已经超出最大限制了
    int     m_nMaxPoolSize;                 // 最大限制值
    int     m_nDeleteStartSize;             // 删除的最低限制值

public:
    BOOL    InitPool(int nInit, int nMaxPoolSize, int nDeleteStartSize = 5)
    {
        SetMaxPoolSize(nMaxPoolSize);
        SetDeleteStartSize(nDeleteStartSize);
        return TPool<T>::InitPool(nInit);
    }

    void    SetMaxPoolSize(int nMaxPoolSize)
    {
        m_nMaxPoolSize      = nMaxPoolSize;
    }
    void    SetDeleteStartSize(int nDeleteStartSize)
    {
        m_nDeleteStartSize  = nDeleteStartSize;
    }

    T*      Pop()
    {
        T *tp = TPool<T>::Pop();

#ifdef _SETACTION
        if((int)(m_setAction.GetCount() + m_stackStay.GetCount()) > m_nMaxPoolSize)
#else
        if((int)(m_nActionCount + m_stackStay.GetCount()) > m_nDeleteStartSize)
#endif
        {
            m_bMaxSizeOver = TRUE;
        }

        return tp;
    }

    void    Push(T *tp)
    {
        TPool<T>::Push(tp);

        if(m_bMaxSizeOver) {
#ifdef _SETACTION
            int nActionPool = (int)m_setAction.GetCount();
#else
            int nActionPool = m_nActionCount;
#endif
            if(nActionPool < m_nDeleteStartSize) {    //
                TGuard<CriticalSection> Guard(&m_csLock);

                m_bMaxSizeOver = FALSE;

                int nDelay = (int)m_stackStay.GetCount();           // 当前剩下项数
                int nTotalPool = nActionPool + nDelay;              // 池总项数
                int nDeletePool = nTotalPool - m_nDeleteStartSize;   // 删除的项数

                for(int i = 0; i < nDeletePool; i++) {               //
                    LPVOID lpData = m_stackStay.GetTail();
                    m_stackStay.RemoveTail();
                    DeleteFunc(lpData);                             // 删除项
                }
            }
        }
    }

protected:
    virtual LPVOID  NewFunc()
    {
        T *pT = new T;

        return (T *)pT;
    }

    virtual void    DeleteFunc(LPVOID pT)
    {
        delete(T *)pT;
        pT  = NULL;
    }
};

#endif  // __POOLMAX_H__
