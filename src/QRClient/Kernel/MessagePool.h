/***********************************************************************
* Module:  MessagePool.h
* Author:  hqr
* Modified: 2016/11/14 15:05:59
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#include "Common/MainFrame.h"
#include "Common/Pool/Pool.h"
#include "kernel/trans_engine_define.h"

class CMessagePool : public TSingleton<CMessagePool>, public TPool<msg_callback_info_t>
{
public:
    CMessagePool(void) { };
    virtual ~CMessagePool(void) { };
};
