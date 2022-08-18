/*
 * Copyright 1993-2018 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef __nvml_header_h_
#define __nvml_header_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVML_CUDA_DRIVER_VERSION_MAJOR(v) ((v)/1000)
#define NVML_CUDA_DRIVER_VERSION_MINOR(v) (((v)%1000)/10)

typedef void *nvmlDevice_t;

/**
 * Return values for NVML API calls.
 */
typedef enum nvmlReturn_enum
{
    NVML_SUCCESS = 0,                   //!< The operation was successful
    NVML_ERROR_NOT_FOUND = 6,
} nvmlReturn_t;

/**
 * Memory allocation information for a device.
 */
typedef struct nvmlMemory_st
{
    unsigned long long total;        //!< Total installed FB memory (in bytes)
    unsigned long long free;         //!< Unallocated FB memory (in bytes)
    unsigned long long used;         //!< Allocated FB memory (in bytes). Note that the driver/GPU always sets aside a small amount of memory for bookkeeping
} nvmlMemory_t;

/**
 * BAR1 Memory allocation Information for a device
 */
typedef struct nvmlBAR1Memory_st
{
    unsigned long long bar1Total;    //!< Total BAR1 Memory (in bytes)
    unsigned long long bar1Free;     //!< Unallocated BAR1 Memory (in bytes)
    unsigned long long bar1Used;     //!< Allocated Used Memory (in bytes)
}nvmlBAR1Memory_t;

/* Information about running compute processes on the GPU */
typedef struct nvmlProcessInfo_st
{
    unsigned int pid;                 //!< Process ID
    unsigned long long usedGpuMemory; //!< Amount of used GPU memory in bytes.
    //!< Under WDDM, \ref NVML_VALUE_NOT_AVAILABLE is always reported
    //!< because Windows KMD manages all the memory and not the NVIDIA driver
} nvmlProcessInfo_t;

typedef struct nvmlUtilization_st
{
    unsigned int gpu;
    unsigned int memory;
} nvmlUtilization_t;

enum nvmlClockType_t {
    NVML_CLOCK_GRAPHICS = 0,
    NVML_CLOCK_SM = 1,
    NVML_CLOCK_MEM = 2,
    NVML_CLOCK_VIDEO = 3
};

// NVML Function pointer typedefs
#define HOOK_FUNC( _ret, _reterr, _func, _args, ... ) typedef _ret ( _func ## _t )( __VA_ARGS__ );
#include "nvml_hook_funcs.inl"
#undef HOOK_FUNC

// Function pointers struct
typedef struct
{
    void *libnvml;

#define HOOK_FUNC( _ret, _reterr, _func, _args, ... ) _func ## _t *_func;
#include "nvml_hook_funcs.inl"
#undef HOOK_FUNC
} nvml_functions_t;

#ifdef __cplusplus
}
#endif

#endif // __nvml_header_h_
