#include "DrmGpuApi.h"
#include "../../../utils/IOUtils.h"
#include <filesystem>

std::map<int, std::string> g_DrmGpuMap;

int DrmGpuApi::GetNumGPUs() {
    if(!std::filesystem::exists("/sys/class/drm"))
        return 0;

    std::regex entryRegex(R"(card(\d+))");
    int count = 0;
    for (const auto& dirEntry : std::filesystem::directory_iterator("/sys/class/drm")) {
        std::smatch matches;
        auto nameStr = dirEntry.path().stem().string();
        if(!std::regex_match(nameStr, matches, entryRegex))
            continue;

        auto vendor = IOUtils::ReadAllText(dirEntry.path().string() + "/device/vendor");
        vendor = Utils::stringTrim(vendor);
        if(vendor == "0x10de") // Nvidia
            continue;

        g_DrmGpuMap[count] = dirEntry.path().string();
        count++;
    }
    return count;
}

std::string DrmGpuApi::GetGPUName(int gpuId) {
    auto path = g_DrmGpuMap[gpuId] + "/device/product_name";
    return IOUtils::ReadAllText(path);
}

std::string DrmGpuApi::GetGPUDriverString(int gpuId) {
    auto linkPath = g_DrmGpuMap[gpuId] + "/device/driver";
    if(!std::filesystem::is_symlink(linkPath))
        return "Unknown";

    auto driverPath = std::filesystem::read_symlink(linkPath);
    return driverPath.stem().string();
}

long DrmGpuApi::GetGPUTotalMemory(int gpuId) {
    auto path = g_DrmGpuMap[gpuId] + "/device/mem_info_vram_total";
    return Utils::stringToLong(IOUtils::ReadAllText(path));
}

long DrmGpuApi::GetGPUFreeMemory(int gpuId) {
    return GetGPUTotalMemory(gpuId) - GetGPUUsedMemory(gpuId);
}

long DrmGpuApi::GetGPUUsedMemory(int gpuId) {
    auto path = g_DrmGpuMap[gpuId] + "/device/mem_info_vram_used";
    return Utils::stringToLong(IOUtils::ReadAllText(path));
}

long DrmGpuApi::GetGPUClock(int gpuId) {
    auto hwMonDir = g_DrmGpuMap[gpuId] + "/hwmon";
    //hwmon for some AMD apu is present at /device/hwmon
    auto hwMonDevDir = g_DrmGpuMap[gpuId] + "/device/hwmon";
    if(std::filesystem::exists(hwMonDir)) {
        for (const auto &dirEntry: std::filesystem::directory_iterator(hwMonDir)) {
            auto inputPath = dirEntry.path().string() + "/freq1_input";
            if (std::filesystem::exists(inputPath))
                return Utils::stringToLong(IOUtils::ReadAllText(inputPath));
        }
    } else if(std::filesystem::exists(hwMonDevDir)) {
        for (const auto &dirEntry: std::filesystem::directory_iterator(hwMonDevDir)) {
            auto inputPath = dirEntry.path().string() + "/freq1_input";
            if (std::filesystem::exists(inputPath))
                return Utils::stringToLong(IOUtils::ReadAllText(inputPath));
        }
    }
    return 0;
}

double DrmGpuApi::GetGPUUsagePercent(int gpuId) {
    auto path = g_DrmGpuMap[gpuId] + "/device/gpu_busy_percent";
    return Utils::stringToDouble(IOUtils::ReadAllText(path));
}

double DrmGpuApi::GetGPUTemperature(int gpuId) {
    auto hwMonDir = g_DrmGpuMap[gpuId] + "/hwmon";
    //hwmon for some AMD apu is present at /device/hwmon
    auto hwMonDevDir = g_DrmGpuMap[gpuId] + "/device/hwmon";
    if(std::filesystem::exists(hwMonDir)) {
        for (const auto &dirEntry: std::filesystem::directory_iterator(hwMonDir)) {
            auto inputPath = dirEntry.path().string() + "/temp1_input";
            if (std::filesystem::exists(inputPath))
                return Utils::stringToDouble(IOUtils::ReadAllText(inputPath));
        }
    } else if(std::filesystem::exists(hwMonDevDir)) {
        for (const auto &dirEntry: std::filesystem::directory_iterator(hwMonDevDir)) {
            auto inputPath = dirEntry.path().string() + "/temp1_input";
            if (std::filesystem::exists(inputPath))
                return Utils::stringToDouble(IOUtils::ReadAllText(inputPath))/1000;
        }
    } else {
        return 0;
    }
}
