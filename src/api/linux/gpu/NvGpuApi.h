#ifndef WSYSMON_NVGPUAPI_H
#define WSYSMON_NVGPUAPI_H

#include <optional>
#include <vector>
#include <spdlog/spdlog.h>

#include "nvml.h"

// https://github.com/mikesart/nvml_info

typedef struct nvgpu_processinfo_st
{
    char name[ 80 ] = { 0 };
    unsigned int pid = 0;
    unsigned long long usedGpuMemory = 0;
} nvgpu_processinfo_t;

typedef struct nvgpu_devinfo_st
{
    char name[ 80 ] = { 0 };
    unsigned int videoClock;
    unsigned int memoryClock;

    nvmlBAR1Memory_t bar1mem = {};
    nvmlMemory_t meminfo = {};
    nvmlUtilization_t utilization = {};

    std::vector< nvgpu_processinfo_t > gpu_processinfos;
    std::vector< nvgpu_processinfo_t > compute_processinfos;
} nvgpu_devinfo_t;

typedef struct nvgpu_info_st
{
    char driver_version[ 256 ] = { 0 };
    char nvml_version[ 256 ] = { 0 };
    int cuda_version = 0;

    std::vector< nvgpu_devinfo_t > devinfos;
} nvgpu_info_t;


class NvGpuApi {
public:
    static std::optional<nvgpu_info_t> GetInfo();
private:
    NvGpuApi() = default;
};


#endif //WSYSMON_NVGPUAPI_H
