#include "../DiskApi.h"

int DiskApi::GetNumDisks() {
    return 0;
}

std::string DiskApi::GetDiskName(int diskId) {
    return {};
}

std::string DiskApi::GetDiskPath(int diskId) {
    return {};
}

DiskType DiskApi::GetDiskType(int diskId) {
    return DiskType::UNKNOWN;
}

long DiskApi::GetDiskTotalSpace(int diskId) {
    return 0;
}

long DiskApi::GetDiskFreeSpace(int diskId) {
    return 0;
}

long DiskApi::GetDiskUsedSpace(int diskId) {
    return 0;
}

double DiskApi::GetDiskUsage(int id) {
    return 0;
}

long DiskApi::GetDiskRead(int id) {
    return 0;
}

long DiskApi::GetDiskWrite(int id) {
    return 0;
}

long DiskApi::GetDiskTotalRead(int id) {
    return 0;
}

long DiskApi::GetDiskTotalWritten(int id) {
    return 0;
}

long DiskApi::ReadStatEntry(int id, int entryIdx) {
    return 0;
}
