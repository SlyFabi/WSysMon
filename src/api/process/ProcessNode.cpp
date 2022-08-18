#include <algorithm>
#include <map>
#include "ProcessNode.h"

ProcessNode::ProcessNode(int pid, int parentPid, const std::string& name, const std::string& status, ProcessUserIds userIds,
                         double cpuUsage, long ramUsage, long diskUsage, long networkUsage,
                         long ioBytesRead, long ioBytesWritten,
                         GPUProcessInfo gpuInfo) {
    m_Pid = pid;
    m_ParentPid = parentPid;
    m_Name = name;
    m_Status = status;
    m_Uids = userIds;
    m_GPUInfo = gpuInfo;
    m_CPUUsage = cpuUsage;
    m_RAMUsage = ramUsage;
    m_DiskUsage = diskUsage;
    m_NetworkUsage = networkUsage;
    m_IOBytesRead = ioBytesRead;
    m_IOBytesWritten = ioBytesWritten;
    m_Parent = nullptr;
}

ProcessNode *ProcessNode::Copy() {
    return new ProcessNode(m_Pid, m_ParentPid, m_Name, m_Status, m_Uids, m_CPUUsage, m_RAMUsage, m_DiskUsage, m_NetworkUsage, m_IOBytesRead, m_IOBytesWritten, m_GPUInfo);
}

ProcessNode *ProcessNode::CopyTree() {
    auto copy = Copy();
    for(auto child : m_Children) {
        auto childCopy = child->CopyTree();
        childCopy->m_Parent = copy;
        copy->AddChild(childCopy);
    }

    return copy;
}

void ProcessNode::AddChild(ProcessNode *node) {
   m_Children.push_back(node);
}

void ProcessNode::RemoveChild(ProcessNode *node) {
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), node), m_Children.end());
}

ProcessNode *ProcessNode::GetParent() {
    return m_Parent;
}

std::vector<ProcessNode *> ProcessNode::GetChildren() {
    return m_Children;
}

int ProcessNode::GetDepth() {
    int depth = 0;
    for(auto child : m_Children) {
        depth += child->GetDepth();
    }

    return depth;
}

int ProcessNode::GetPid() const {
    return m_Pid;
}

int ProcessNode::GetParentPid() const {
    return m_ParentPid;
}

std::string ProcessNode::GetName() {
    return m_Name;
}

std::string ProcessNode::GetStatus() {
    return m_Status;
}

ProcessUserIds ProcessNode::GetUserIds() {
    return m_Uids;
}

GPUProcessInfo ProcessNode::GetGPUInfo() {
    return m_GPUInfo;
}

double ProcessNode::GetCPUUsage() const {
    return m_CPUUsage;
}

long ProcessNode::GetRAMUsage() const {
    return m_RAMUsage;
}

long ProcessNode::GetDiskUsage() const {
    return m_DiskUsage;
}

long ProcessNode::GetNetworkUsage() const {
    return m_NetworkUsage;
}

long ProcessNode::GetIOBytesRead() const {
    return m_IOBytesRead;
}

long ProcessNode::GetIOBytesWritten() const {
    return m_IOBytesWritten;
}

void ProcessNode::SetParent(ProcessNode *node) {
    if(m_Parent != nullptr)
        m_Parent->RemoveChild(node);

    m_Parent = node;
    if(m_Parent != nullptr)
        m_Parent->AddChild(this);
}

void ProcessNode::SetParentPid(int ppid) {
    m_ParentPid = ppid;
}

std::vector<ProcessNode *> ProcessNode::FlatTree(bool withChildren) {
    std::vector<ProcessNode *> result;
    result.push_back(this);
    if(withChildren) {
        for(auto child : m_Children) {
            auto childList = child->FlatTree(withChildren);
            result.insert(std::end(result), std::begin(childList), std::end(childList));
        }
    }
    return result;
}

ProcessNode * ProcessNode::FlatDepth(int depth) {
    // ToDo
    return nullptr;
}

double ProcessNode::CalculateCPUUsageSums() {
    for(auto child : m_Children) {
        m_CPUUsage += child->CalculateCPUUsageSums();
    }
    return m_CPUUsage;
}

long ProcessNode::CalculateRAMUsageSums() {
    for(auto child : m_Children) {
        m_RAMUsage += child->CalculateRAMUsageSums();
    }
    return m_RAMUsage;
}

long ProcessNode::CalculateGPUUsageSums() {
    for(auto child : m_Children) {
        m_GPUInfo.memoryUsage += child->CalculateGPUUsageSums();
    }
    return m_GPUInfo.memoryUsage;
}

void ProcessNode::SortTree() {
    std::sort(m_Children.begin(), m_Children.end(), [](const ProcessNode *a, const ProcessNode *b) -> bool {
        auto aName = Utils::stringToLower(a->m_Name);
        auto bName = Utils::stringToLower(b->m_Name);
        return bName.compare(aName) > 0;
    });

    for(auto child : m_Children)
        child->SortTree();
}

void ProcessNode::Reduce(std::vector<ProcessNode *>& procList, const std::vector<ProcessNode *>& toRemove) {
    for(auto rem : toRemove) {
        auto found = std::find_if(procList.begin(), procList.end(),[&](ProcessNode *n) -> bool { return n->GetPid() == rem->GetPid(); });
        if(found != std::end(procList)) {
            auto procNode = *found;
            /*for(auto procChild : procNode->m_Children) {
                procChild->SetParent(procNode->m_Parent);
                if(procChild->m_Parent != nullptr)
                    procChild->SetParentPid(procChild->m_Parent->GetPid());
            }*/

            procList.erase(found);
            delete procNode;
        }
    }
}

ProcessNode *ProcessNode::BuildTree(const std::vector<ProcessNode *>& procList) {
    std::map<int, ProcessNode *> pidMap;
    for(auto proc : procList) {
        pidMap.insert(std::make_pair(proc->GetPid(), proc));
    }

    auto root = new ProcessNode(0, -1, "ROOT", "", ProcessUserIds(), 0, 0, 0, 0, 0, 0, {});
    for(auto proc : procList) {
        ProcessNode *parent;
        if(proc->GetParentPid() > 0) {
            parent = pidMap[proc->GetParentPid()];
            if(parent == nullptr)
                parent = root;
        } else {
            parent = root;
        }

        proc->SetParent(parent);
    }

    return root;
}

ProcessNode *ProcessNode::BuildFlatTree(const std::vector<ProcessNode *> &procList) {
    auto root = new ProcessNode(0, -1, "ROOT", "", ProcessUserIds(), 0, 0, 0, 0, 0, 0, {});
    for(auto proc : procList) {
        proc->SetParent(root);
    }
    return root;
}
