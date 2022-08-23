#ifndef WSYSMON_DRMGPUAPI_H
#define WSYSMON_DRMGPUAPI_H

#include "../../../utils/Utils.h"

class DrmGpuApi {
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
};


#endif //WSYSMON_DRMGPUAPI_H
