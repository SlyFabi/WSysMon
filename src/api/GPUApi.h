#ifndef WSYSMON_GPUAPI_H
#define WSYSMON_GPUAPI_H

#include "../utils/Utils.h"
#include "linux/gpu/NvGpuApi.h"
#include "linux/gpu/DrmGpuApi.h"

struct GPUProcessInfo {
    long memoryUsage;
};

class GPUApi {
public:
    static int GetNumGPUs();

    static std::string GetGPUName(int gpuId);
    static std::string GetGPUDriverString(int gpuId);

    static long GetGPUTotalMemory(int gpuId);
    static long GetGPUFreeMemory(int gpuId);
    static long GetGPUUsedMemory(int gpuId);

    static long GetGPUClock(int gpuId);
    static double GetGPUUsagePercent(int gpuId);
    static double GetGPUTemperature(int gpuId);

    static GPUProcessInfo GetGPUProcessInfo(nvgpu_info_t nvidiaInfo, int gpuId, int pid);

private:
    GPUApi() = default;
};


#endif //WSYSMON_GPUAPI_H
