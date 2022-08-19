#include "../GPUApi.h"
#include "gpu/NvGpuApi.h"

#define MB_TO_B 1000000
#define MHZ_TO_HZ 1000000

int GPUApi::GetNumGPUs() {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return (int)nvInfo.value().devinfos.size();
}

std::string GPUApi::GetGPUName(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return {};

    return nvInfo.value().devinfos[gpuId].name;
}

std::string GPUApi::GetGPUDriverString(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return {};

    return nvInfo.value().driver_version;
}

long GPUApi::GetGPUTotalMemory(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return (long)nvInfo.value().devinfos[gpuId].meminfo.total;
}

long GPUApi::GetGPUFreeMemory(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return (long)nvInfo.value().devinfos[gpuId].meminfo.free;
}

long GPUApi::GetGPUUsedMemory(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return (long)nvInfo.value().devinfos[gpuId].meminfo.used;
}

long GPUApi::GetGPUClock(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return (long)nvInfo.value().devinfos[gpuId].videoClock * MHZ_TO_HZ;
}

double GPUApi::GetGPUUsagePercent(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return nvInfo.value().devinfos[gpuId].utilization.gpu;
}

double GPUApi::GetGPUTemperature(int gpuId) {
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;
    return nvInfo.value().devinfos[gpuId].gpuTemperature;
}

GPUProcessInfo GPUApi::GetGPUProcessInfo(nvgpu_info_t nvidiaInfo, int gpuId, int pid) {
    auto nvGpu = nvidiaInfo.devinfos[gpuId];
    for(auto proc : nvGpu.gpu_processinfos)
        if(proc.pid == pid)
            return {(long)proc.usedGpuMemory};
    for(auto proc : nvGpu.compute_processinfos)
        if(proc.pid == pid)
            return {(long)proc.usedGpuMemory};

    return {};
}
