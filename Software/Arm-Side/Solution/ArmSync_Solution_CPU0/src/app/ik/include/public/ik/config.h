/* IK 1.1 — Configuration for RA8P1 (ARM Cortex-M85, GCC bare-metal) */
#ifndef IK_CONFIG_H
#define IK_CONFIG_H

/* --- stdint --- */
#define IK_HAVE_STDINT_H
#ifdef IK_HAVE_STDINT_H
#   include <stdint.h>
#else
#   include "ik/pstdint.h"
#endif

#include "ik/retcodes.h"

/* --- Precision --- */
#define IK_PRECISION_FLOAT
typedef float ikreal;
typedef ikreal ikreal_t;  /* alias for source compatibility */
#include <float.h>
#define IK_EPSILON FLT_EPSILON

/* --- Word size: 32-bit ARM → 2^2 = 4 --- */
#define IK_CPU_WORD_SIZE 2

/* --- GCC attributes --- */
#define IK_WARN_UNUSED __attribute__((warn_unused_result))

/* --- Visibility (static library) --- */
#define IK_HELPER_API_EXPORT
#define IK_HELPER_API_IMPORT
#define IK_HELPER_API_LOCAL
#define IK_PUBLIC_API
#define IK_PRIVATE_API

/* --- C linkage --- */
#ifdef __cplusplus
#   define C_BEGIN extern "C" {
#   define C_END }
#else
#   define C_BEGIN
#   define C_END
#endif

/* --- Vtable macros (manual stubs — only used by .v files at build time) --- */
#define IK_INTERFACE(x)  struct ik_##x##_t
#define IK_IMPLEMENT(x, base) /* processed by script */
#define IK_OVERRIDE(x)   /* processed by script */

#endif
