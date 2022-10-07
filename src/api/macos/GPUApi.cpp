#include "../GPUApi.h"

int GPUApi::GetNumGPUs() {
    return 0;
}

std::string GPUApi::GetGPUName(int gpuId) {
    return {};
}

std::string GPUApi::GetGPUDriverString(int gpuId) {
    return {};
}

long GPUApi::GetGPUTotalMemory(int gpuId) {
    return 0;
}

long GPUApi::GetGPUFreeMemory(int gpuId) {
    return 0;
}

long GPUApi::GetGPUUsedMemory(int gpuId) {
    return 0;
}

long GPUApi::GetGPUClock(int gpuId) {
    return 0;
}

double GPUApi::GetGPUUsagePercent(int gpuId) {
    return 0;
}

double GPUApi::GetGPUTemperature(int gpuId) {
    return 0;
}

GPUProcessInfo GPUApi::GetGPUProcessInfo(nvgpu_info_t nvidiaInfo, int gpuId, int pid) {
    return {};
}