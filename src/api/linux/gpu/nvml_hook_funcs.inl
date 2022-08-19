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

HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlInit, (), void )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlInit_v2, (), void )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlShutdown, (), void )

HOOK_FUNC( const char *, "nvmlErrorString function not found", nvmlErrorString, ( result ), nvmlReturn_t result )

HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetCount, ( deviceCount ), unsigned int *deviceCount )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetHandleByIndex, ( index, device ), unsigned int index, nvmlDevice_t *device )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetMemoryInfo, ( device, memory ), nvmlDevice_t device, nvmlMemory_t *memory )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetComputeRunningProcesses, ( device, infoCount, infos ), nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlSystemGetProcessName, ( pid, name, length ), unsigned int pid, char *name, unsigned int length )

HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetGraphicsRunningProcesses, ( device, infoCount, infos ), nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetBAR1MemoryInfo, ( device, bar1Memory ), nvmlDevice_t device, nvmlBAR1Memory_t *bar1Memory )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetName, ( device, name, length ), nvmlDevice_t device, char* name, unsigned int  length )

HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetClockInfo, ( device, type, clock ), nvmlDevice_t device, nvmlClockType_t type, unsigned int* clock )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetUtilizationRates, ( device, utilization ), nvmlDevice_t device, nvmlUtilization_t* utilization )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlDeviceGetTemperature, ( device, sensorType, temp ), nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int* temp )

HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlSystemGetDriverVersion, ( version, length ), char *version, unsigned int length )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlSystemGetNVMLVersion, ( version, length ), char *version, unsigned int length )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlSystemGetCudaDriverVersion, ( cudaDriverVersion ), int *cudaDriverVersion )
HOOK_FUNC( nvmlReturn_t, NVML_ERROR_NOT_FOUND, nvmlSystemGetCudaDriverVersion_v2, ( cudaDriverVersion ), int *cudaDriverVersion )