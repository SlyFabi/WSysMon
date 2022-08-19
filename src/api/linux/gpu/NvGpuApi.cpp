/*
 * Copyright 2019 Michael Sartain
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "NvGpuApi.h"

#include <cstdio>
#include <dlfcn.h>

int nvml_functions_init( nvml_functions_t &nvmlfuncs );
void nvml_functions_shutdown( nvml_functions_t &nvmlfuncs );

nvmlReturn_t nvgpu_get_info( const nvml_functions_t &nvmlfuncs, nvgpu_info_t &nvgpu_info );

/*
 * Functions
 */

// Declare empty error functions if symbol isn't found in dso
#define HOOK_FUNC( _ret, _reterr, _func, _args, ... ) static _ret ( _func ## _null )( __VA_ARGS__ ) { return _reterr; }
#include "nvml_hook_funcs.inl"
#undef HOOK_FUNC

int nvml_functions_init(nvml_functions_t &nvmlfuncs) {
    // Try to load nvml dso
    void *libnvml = dlopen("libnvidia-ml.so.1", RTLD_NOW | RTLD_GLOBAL);
    if (!libnvml)
        libnvml = dlopen("libnvidia-ml.so", RTLD_NOW | RTLD_GLOBAL);
    if (!libnvml) {
        spdlog::error("nvml_functions_init failed ({})", dlerror());
        return -1;
    }

    // Get nvml function addresses
#define HOOK_FUNC( _ret, _reterr, _func, _args, ... )           \
    nvmlfuncs._func = ( _func##_t * )dlsym( libnvml, #_func );  \
    if ( !nvmlfuncs._func )                                     \
        nvmlfuncs._func = _func ## _null;

    do {
#include "nvml_hook_funcs.inl"
    } while ( 0 );

#undef HOOK_FUNC

    // Use v2 functions if available
    if (nvmlfuncs.nvmlInit_v2)
        nvmlfuncs.nvmlInit = nvmlfuncs.nvmlInit_v2;

    if (nvmlfuncs.nvmlSystemGetCudaDriverVersion_v2)
        nvmlfuncs.nvmlSystemGetCudaDriverVersion = nvmlfuncs.nvmlSystemGetCudaDriverVersion_v2;

    nvmlfuncs.libnvml = libnvml;
    return 0;
}

void nvml_functions_shutdown(nvml_functions_t &nvmlfuncs) {
    if (nvmlfuncs.libnvml) {
        dlclose(nvmlfuncs.libnvml);
        nvmlfuncs.libnvml = NULL;
    }
}

#define NVML_CALL(_ret, _func, ...)                                                       \
    do {                                                                                    \
        _ret = nvmlfuncs._func(__VA_ARGS__);                                              \
        if (_ret != NVML_SUCCESS) {                                                        \
            spdlog::error("{} failed ({})", #_func, nvmlfuncs.nvmlErrorString(_ret));      \
        }                                                                                   \
    } while ( 0 )

static void nvgpu_get_process_info(const nvml_functions_t &nvmlfuncs, nvmlDevice_t nvmldev, std::vector< nvgpu_processinfo_t > &dst, bool doCompute) {
    nvmlReturn_t ret;

    unsigned int process_count = 256;
    nvmlProcessInfo_t process_infos[ 256 ];

    if (doCompute)
        NVML_CALL( ret, nvmlDeviceGetComputeRunningProcesses, nvmldev, &process_count, process_infos );
    else
        NVML_CALL( ret, nvmlDeviceGetGraphicsRunningProcesses, nvmldev, &process_count, process_infos );

    if (!ret && process_count) {
        dst.resize(process_count);
        for (unsigned int i = 0; i < process_count; i++) {
            nvgpu_processinfo_t &procinfo = dst[ i ];
            procinfo.pid = process_infos[ i ].pid;
            procinfo.usedGpuMemory = process_infos[ i ].usedGpuMemory;

            NVML_CALL( ret, nvmlSystemGetProcessName, process_infos[ i ].pid, procinfo.name, sizeof( procinfo.name ) );
        }
    }
}

nvmlReturn_t nvgpu_get_info(const nvml_functions_t &nvmlfuncs, nvgpu_info_t &nvgpu_info) {
    nvmlReturn_t ret = nvmlfuncs.nvmlInit();
    if (ret != NVML_SUCCESS) {
        spdlog::error("nvmlInit failed ({})", nvmlfuncs.nvmlErrorString(ret));
    } else {
        NVML_CALL(ret, nvmlSystemGetNVMLVersion, nvgpu_info.nvml_version, sizeof(nvgpu_info.nvml_version));
        NVML_CALL(ret, nvmlSystemGetDriverVersion, nvgpu_info.driver_version, sizeof(nvgpu_info.driver_version));
        NVML_CALL(ret, nvmlSystemGetCudaDriverVersion, &nvgpu_info.cuda_version);

        unsigned int device_count = 0;
        NVML_CALL(ret, nvmlDeviceGetCount, &device_count);

        if (ret == NVML_SUCCESS) {
            for (unsigned int i = 0; i < device_count; i++) {
                nvmlDevice_t nvmldev;
                nvgpu_devinfo_t devinfo;

                NVML_CALL(ret, nvmlDeviceGetHandleByIndex, i, &nvmldev);

                NVML_CALL(ret, nvmlDeviceGetName, nvmldev, devinfo.name, sizeof(devinfo.name));
                NVML_CALL(ret, nvmlDeviceGetClockInfo, nvmldev, nvmlClockType_t::NVML_CLOCK_GRAPHICS, &devinfo.videoClock);
                NVML_CALL(ret, nvmlDeviceGetClockInfo, nvmldev, nvmlClockType_t::NVML_CLOCK_MEM, &devinfo.memoryClock);
                NVML_CALL(ret, nvmlDeviceGetUtilizationRates, nvmldev, &devinfo.utilization);
                NVML_CALL(ret, nvmlDeviceGetBAR1MemoryInfo, nvmldev, &devinfo.bar1mem);
                NVML_CALL(ret, nvmlDeviceGetMemoryInfo, nvmldev, &devinfo.meminfo);
                NVML_CALL(ret, nvmlDeviceGetTemperature, nvmldev, nvmlTemperatureSensors_t::NVML_TEMPERATURE_GPU, &devinfo.gpuTemperature);

                nvgpu_get_process_info(nvmlfuncs, nvmldev, devinfo.gpu_processinfos, false);
                nvgpu_get_process_info(nvmlfuncs, nvmldev, devinfo.compute_processinfos, true);

                nvgpu_info.devinfos.emplace_back(devinfo);
            }
        }
        nvmlfuncs.nvmlShutdown();
    }
    return ret;
}

nvml_functions_t g_NvmlFuncs{};
bool g_NvHasError = false;

std::optional<nvgpu_info_t> NvGpuApi::GetInfo() {
    if(g_NvHasError)
        return std::nullopt;

    if(g_NvmlFuncs.libnvml == nullptr) {
        if(nvml_functions_init(g_NvmlFuncs) != 0) {
            g_NvmlFuncs.libnvml = nullptr;
            g_NvHasError = true;
            return std::nullopt;
        }
    }

    nvgpu_info_t nvgpu_info;
    if(nvgpu_get_info(g_NvmlFuncs, nvgpu_info) == 0)
        return nvgpu_info;

    //nvml_functions_shutdown( nvmlfuncs ); ToDo
    return std::nullopt;
}
