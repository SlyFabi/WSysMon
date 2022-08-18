#ifndef WSYSMON_DISKAPI_H
#define WSYSMON_DISKAPI_H

#include <string>
#include "../utils/Utils.h"

enum DiskType {
    UNKNOWN,
    HDD,
    SSD,
    NVME,
    RAID
};

class DiskApi {
public:
    static int GetNumDisks();

    static std::string GetDiskName(int diskId);
    static std::string GetDiskPath(int diskId);
    static DiskType GetDiskType(int diskId);

    static long GetDiskTotalSpace(int diskId);
    static long GetDiskFreeSpace(int diskId);
    static long GetDiskUsedSpace(int diskId);

    static double GetDiskUsage(int id);

    static long GetDiskRead(int id);
    static long GetDiskWrite(int id);

    static long GetDiskTotalRead(int id);
    static long GetDiskTotalWritten(int id);

private:
    static long ReadStatEntry(int id, int entryIdx);

    DiskApi() = default;
};


#endif //WSYSMON_GPUAPI_H
