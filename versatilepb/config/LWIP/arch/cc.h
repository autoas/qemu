/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * lwIP architecture port file for the bare metal ARM target
 */
#ifndef __ARCH_CC_H
#define __ARCH_CC_H
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "Std_Types.h"
/* ================================ [ MACROS    ] ============================================== */
#define LWIP_NO_UNISTD_H 1
#define LWIP_NO_INTTYPES_H 0

/* std_printf is provided by the LibC library of this project */
int std_printf(const char *fmt, ...);

#define LWIP_PLATFORM_DIAG(x)                                                                      \
  do {                                                                                             \
    std_printf x;                                                                                  \
  } while (0)

#define LWIP_PLATFORM_ASSERT(x)                                                                    \
  do {                                                                                             \
    std_printf("lwIP assert \"%s\" at line %d in %s\n", x, __LINE__, __FILE__);                    \
    while (1) {                                                                                    \
    }                                                                                              \
  } while (0)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#define LWIP_PLATFORM_BYTESWAP 0
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __ARCH_CC_H */
