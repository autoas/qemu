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
#include "heap.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define rt_memset memset
#define rt_memcpy memcpy

#define rt_malloc malloc
#define rt_free free

#define rt_malloc_align(size, align) memalign(align, size)
#define rt_free_align(ptr) free(ptr)

#define RT_ASSERT asAssert
#define rt_kprintf printf
#define rt_snprintf snprintf

void rt_thread_yield(void);

/*
 * device (I/O) system interface
 */
rt_device_t rt_device_find(const char *name);
rt_err_t rt_device_register(rt_device_t dev, const char *name, rt_uint16_t flags);
rt_err_t rt_device_unregister(rt_device_t dev);

#ifdef __cplusplus
}
#endif

#endif
