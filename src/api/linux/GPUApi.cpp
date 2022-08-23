#include "../GPUApi.h"
#include "gpu/NvGpuApi.h"

#define MB_TO_B 1000000
#define MHZ_TO_HZ 1000000

int g_NumDrmGpus = 0;

int GPUApi::GetNumGPUs() {
    g_NumDrmGpus = DrmGpuApi::GetNumGPUs();
    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return g_NumDrmGpus;

    return g_NumDrmGpus + (int)nvInfo.value().devinfos.size();
}

std::string GPUApi::GetGPUName(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUName(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return {};

    return nvInfo.value().devinfos[gpuId - g_NumDrmGpus].name;
}

std::string GPUApi::GetGPUDriverString(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUDriverString(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return {};

    return nvInfo.value().driver_version;
}

long GPUApi::GetGPUTotalMemory(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUTotalMemory(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return (long)nvInfo.value().devinfos[gpuId - g_NumDrmGpus].meminfo.total;
}

long GPUApi::GetGPUFreeMemory(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUFreeMemory(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return (long)nvInfo.value().devinfos[gpuId - g_NumDrmGpus].meminfo.free;
}

long GPUApi::GetGPUUsedMemory(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUUsedMemory(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return (long)nvInfo.value().devinfos[gpuId - g_NumDrmGpus].meminfo.used;
}

long GPUApi::GetGPUClock(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUClock(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return (long)nvInfo.value().devinfos[gpuId - g_NumDrmGpus].videoClock * MHZ_TO_HZ;
}

double GPUApi::GetGPUUsagePercent(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUUsagePercent(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return nvInfo.value().devinfos[gpuId - g_NumDrmGpus].utilization.gpu;
}

double GPUApi::GetGPUTemperature(int gpuId) {
    if(gpuId < g_NumDrmGpus)
        return DrmGpuApi::GetGPUTemperature(gpuId);

    auto nvInfo = NvGpuApi::GetInfo();
    if(nvInfo == std::nullopt)
        return 0;

    return nvInfo.value().devinfos[gpuId - g_NumDrmGpus].gpuTemperature;
}

GPUProcessInfo GPUApi::GetGPUProcessInfo(nvgpu_info_t nvidiaInfo, int gpuId, int pid) {
    if(gpuId < g_NumDrmGpus)
        return {};

    auto nvGpu = nvidiaInfo.devinfos[gpuId - g_NumDrmGpus];
    for(auto proc : nvGpu.gpu_processinfos)
        if(proc.pid == pid)
            return {(long)proc.usedGpuMemory};
    for(auto proc : nvGpu.compute_processinfos)
        if(proc.pid == pid)
            return {(long)proc.usedGpuMemory};

    return {};
}
