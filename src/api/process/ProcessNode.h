#ifndef WSYSMON_PROCESSNODE_H
#define WSYSMON_PROCESSNODE_H

#include <string>
#include <vector>

#include "../ProcessesApi.h"
#include "../GPUApi.h"

class ProcessNode {
public:
    ProcessNode(int pid, int parentPid, const std::string& name, const std::string& status, ProcessUserIds userIds,
                double cpuUsage, long ramUsage, long diskUsage, long networkUsage,
                long ioBytesRead, long ioBytesWritten,
                GPUProcessInfo gpuInfo);

    ProcessNode *Copy();
    ProcessNode *CopyTree();

    void AddChild(ProcessNode *node);
    void RemoveChild(ProcessNode *node);

    ProcessNode *GetParent();
    std::vector<ProcessNode *> GetChildren();
    int GetDepth();

    [[nodiscard]] int GetPid() const;
    [[nodiscard]] int GetParentPid() const;

    std::string GetName();
    std::string GetStatus();
    ProcessUserIds GetUserIds();
    GPUProcessInfo GetGPUInfo();

    [[nodiscard]] double GetCPUUsage() const;
    [[nodiscard]] long GetRAMUsage() const;
    [[nodiscard]] long GetDiskUsage() const;
    [[nodiscard]] long GetNetworkUsage() const;

    [[nodiscard]] long GetIOBytesRead() const;
    [[nodiscard]] long GetIOBytesWritten() const;

    std::vector<ProcessNode *> FlatTree(bool withChildern = true);
    ProcessNode *FlatDepth(int depth);

    double CalculateCPUUsageSums();
    long CalculateRAMUsageSums();
    long CalculateGPUUsageSums();
    void SortTree();

    static void Reduce(std::vector<ProcessNode *>& procList, const std::vector<ProcessNode *>& toRemove);

    void SetParent(ProcessNode *node);
    void SetParentPid(int ppid);

    static ProcessNode *BuildTree(const std::vector<ProcessNode *>& procList);
    static ProcessNode *BuildFlatTree(const std::vector<ProcessNode *>& procList);

private:
    ProcessNode *m_Parent;
    std::vector<ProcessNode *> m_Children{};

    int m_Pid;
    int m_ParentPid;

    std::string m_Name;
    std::string m_Status;
    ProcessUserIds m_Uids{};
    GPUProcessInfo m_GPUInfo{};

    double m_CPUUsage;
    long m_RAMUsage;
    long m_DiskUsage;
    long m_NetworkUsage;

    long m_IOBytesRead;
    long m_IOBytesWritten;
};


#endif //WSYSMON_PROCESSNODE_H
