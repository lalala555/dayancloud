#ifndef __POOLMAX_H__
#define __POOLMAX_H__

#include "Pool.h"

// ���ռ����Ƶ�Pool
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
    BOOL    m_bMaxSizeOver;                 // �Ƿ��Ѿ��������������
    int     m_nMaxPoolSize;                 // �������ֵ
    int     m_nDeleteStartSize;             // ɾ�����������ֵ

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

                int nDelay = (int)m_stackStay.GetCount();           // ��ǰʣ������
                int nTotalPool = nActionPool + nDelay;              // ��������
                int nDeletePool = nTotalPool - m_nDeleteStartSize;   // ɾ��������

                for(int i = 0; i < nDeletePool; i++) {               //
                    LPVOID lpData = m_stackStay.GetTail();
                    m_stackStay.RemoveTail();
                    DeleteFunc(lpData);                             // ɾ����
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
