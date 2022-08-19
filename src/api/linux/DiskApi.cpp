#include "../DiskApi.h"
#include "../SystemInfoApi.h"

#include <filesystem>

struct DiskApiIOSnapshot {
    long bytes;
    long timeMs;
};

std::map<int, std::string> g_DiskMap;

std::map<int, DiskApiIOSnapshot> g_IOSnapshotsRead;
std::map<int, DiskApiIOSnapshot> g_IOSnapshotsWrite;
std::map<int, DiskApiIOSnapshot> g_UsageSnapshots;

int DiskApi::GetNumDisks() {
    g_DiskMap.clear();
    g_IOSnapshotsRead.clear();
    g_IOSnapshotsWrite.clear();
    g_UsageSnapshots.clear();

    int count = 0;
    for (const auto& dirEntry : std::filesystem::directory_iterator("/sys/block")) {
        if(!Utils::stringStartsWith(dirEntry.path().stem(), "sd")
        && !Utils::stringStartsWith(dirEntry.path().stem(), "hd")
        && !Utils::stringStartsWith(dirEntry.path().stem(), "md")
        && !Utils::stringStartsWith(dirEntry.path().stem(), "nvme"))
            continue;

        g_DiskMap[count] = dirEntry.path().string();
        count++;
    }
    return count;
}

std::string DiskApi::GetDiskName(int diskId) {
    auto diskPath = g_DiskMap[diskId];
    auto modelPath = diskPath + "/device/model";
    if(std::filesystem::exists(modelPath))
        return Utils::stringTrim(IOUtils::ReadAllText(modelPath));

    auto path = std::filesystem::path(diskPath);
    return path.stem().string();
}

std::string DiskApi::GetDiskPath(int diskId) {
    auto diskPath = g_DiskMap[diskId];
    auto path = std::filesystem::path(diskPath);
    return "/dev/" + path.stem().string();
}

DiskType DiskApi::GetDiskType(int diskId) {
    auto diskPath = g_DiskMap[diskId];
    auto path = std::filesystem::path(diskPath);
    if(Utils::stringStartsWith(path.stem(), "md"))
        return RAID;
    if(Utils::stringStartsWith(path.stem(), "nvme"))
        return NVME;

    auto diskType = UNKNOWN;
    auto isHdd = Utils::stringToLong(IOUtils::ReadAllText(diskPath + "/queue/rotational"));
    if(isHdd == 1)
        diskType = HDD;
    else if(isHdd == 0)
        diskType = SSD;
    return diskType;
}

long DiskApi::GetDiskTotalSpace(int diskId) {
    auto diskPath = g_DiskMap[diskId];
    auto sizeStr = IOUtils::ReadAllText(diskPath + "/size");
    return Utils::stringToLong(sizeStr) * 512;
}

long DiskApi::GetDiskFreeSpace(int diskId) {
    return 0;
}

long DiskApi::GetDiskUsedSpace(int diskId) {
    return 0;
}

double DiskApi::GetDiskUsage(int id) {
    if(!Utils::mapContains(g_UsageSnapshots, id)) {
        auto ioTicks = ReadStatEntry(id, 9);
        DiskApiIOSnapshot snapshot{};
        snapshot.bytes = ioTicks;
        snapshot.timeMs = Utils::GetCurrentTimeMS();
        g_UsageSnapshots[id] = snapshot;
        return 0;
    }

    auto lastUsage = g_UsageSnapshots[id];
    auto ioTicks = ReadStatEntry(id, 9);

    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastUsage.timeMs) / 1000.;
    auto tickDiff = ioTicks - lastUsage.bytes;

    DiskApiIOSnapshot snapshot{};
    snapshot.bytes = ioTicks;
    snapshot.timeMs = Utils::GetCurrentTimeMS();
    g_UsageSnapshots[id] = snapshot;
    return (double)tickDiff / ((double)timeDiffS * 10.);
}

long DiskApi::GetDiskRead(int id) {
    if(!Utils::mapContains(g_IOSnapshotsRead, id)) {
        auto recv = GetDiskTotalRead(id);
        DiskApiIOSnapshot snapshot{};
        snapshot.bytes = recv;
        snapshot.timeMs = Utils::GetCurrentTimeMS();
        g_IOSnapshotsRead[id] = snapshot;
        return 0;
    }

    auto recv = GetDiskTotalRead(id);
    auto lastBytes = g_IOSnapshotsRead[id];
    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastBytes.timeMs) / 1000.;
    auto bytesDiff = recv - lastBytes.bytes;

    auto mul = 1.;
    if(timeDiffS < 1)
        mul += timeDiffS;
    bytesDiff = (long)((double)bytesDiff * mul);

    DiskApiIOSnapshot snapshot{};
    snapshot.bytes = recv;
    snapshot.timeMs = Utils::GetCurrentTimeMS();
    g_IOSnapshotsRead[id] = snapshot;
    return bytesDiff;
}

long DiskApi::GetDiskWrite(int id) {
    if(!Utils::mapContains(g_IOSnapshotsWrite, id)) {
        auto sent = GetDiskTotalWritten(id);
        DiskApiIOSnapshot snapshot{};
        snapshot.bytes = sent;
        snapshot.timeMs = Utils::GetCurrentTimeMS();
        g_IOSnapshotsWrite[id] = snapshot;
        return 0;
    }

    auto sent = GetDiskTotalWritten(id);
    auto lastBytes = g_IOSnapshotsWrite[id];
    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastBytes.timeMs) / 1000.;
    auto bytesDiff = sent - lastBytes.bytes;

    auto mul = 1.;
    if(timeDiffS < 1)
        mul += timeDiffS;
    bytesDiff = (long)((double)bytesDiff * mul);

    DiskApiIOSnapshot snapshot{};
    snapshot.bytes = sent;
    snapshot.timeMs = Utils::GetCurrentTimeMS();
    g_IOSnapshotsWrite[id] = snapshot;
    return bytesDiff;
}

long DiskApi::GetDiskTotalRead(int id) {
    return ReadStatEntry(id, 0) * 1024;
}

long DiskApi::GetDiskTotalWritten(int id) {
    return ReadStatEntry(id, 1) * 1024;
}

long DiskApi::ReadStatEntry(int id, int entryIdx) {
    auto file = fopen((g_DiskMap[id] + "/stat").c_str(), "r");
    if (file == nullptr) {
        return false;
    }

    long sectorsRead;
    long sectorsWritten;
    long tmpL;
    long busyTime;
    int scanResult = fscanf(file,
                            "\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld",
                            &tmpL, &tmpL, &sectorsRead, &tmpL, &tmpL, &tmpL,
                            &sectorsWritten, &tmpL, &tmpL, &busyTime, &tmpL);
    if(scanResult != 11) {
        fclose(file);
        return false;
    }

    fclose(file);
    if(entryIdx == 0)
        return sectorsRead;
    if(entryIdx == 9)
        return busyTime;
    return sectorsWritten;
}
