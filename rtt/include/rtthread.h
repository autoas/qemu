/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

#include <rtconfig.h>
#include <rtdef.h>
#include <string.h>
#include "Std_Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define rt_memset memset

#define RT_ASSERT asAssert
#define rt_kprintf printf

#ifdef __cplusplus
}
#endif

#endif
