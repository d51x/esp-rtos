#pragma once

#ifndef __IOT_DEBUG_H__
#define __IOT_DEBUG_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#ifdef CONFIG_DEBUG_UART1


    void enable_debug_uart1();
    void userlog(const char *fmt, ...);
//#endif


#endif

