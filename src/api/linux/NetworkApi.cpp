#include "../NetworkApi.h"
#include "../../utils/IOUtils.h"

#include <filesystem>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

struct NetworkApiIOSnapshot {
    long bytes;
    long timeMs;
};

std::map<int, std::string> g_InterfaceMap;

std::map<int, NetworkApiIOSnapshot> g_IOSnapshotsRecv;
std::map<int, NetworkApiIOSnapshot> g_IOSnapshotsSend;

int NetworkApi::GetNumInterfaces() {
    g_InterfaceMap.clear();
    g_IOSnapshotsRecv.clear();
    g_IOSnapshotsSend.clear();

    int count = 0;
    for (const auto& dirEntry : std::filesystem::directory_iterator("/sys/class/net")) {
        if(dirEntry.path().stem() == "lo")
            continue;

        auto state = IOUtils::ReadAllText(dirEntry.path().string() + "/operstate");
        if(Utils::stringTrim(state) != "up")
            continue;

        g_InterfaceMap[count] = dirEntry.path().string();
        count++;
    }

    return count;
}

std::string NetworkApi::GetInterfaceName(int id) {
    auto interfacePath = g_InterfaceMap[id];
    auto path = std::filesystem::path(interfacePath);
    return path.stem().string();
}

long NetworkApi::GetInterfaceReceive(int id) {
    if(!Utils::mapContains(g_IOSnapshotsRecv, id)) {
        auto recv = GetInterfaceTotalReceived(id);
        NetworkApiIOSnapshot snapshot{};
        snapshot.bytes = recv;
        snapshot.timeMs = Utils::GetCurrentTimeMS();
        g_IOSnapshotsRecv[id] = snapshot;

        return 0;
    }

    auto recv = GetInterfaceTotalReceived(id);

    auto lastBytes = g_IOSnapshotsRecv[id];
    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastBytes.timeMs) / 1000.;
    auto bytesDiff = recv - lastBytes.bytes;

    auto mul = 1.;
    if(timeDiffS < 1)
        mul += timeDiffS;

    bytesDiff = (long)((double)bytesDiff * mul);

    NetworkApiIOSnapshot snapshot{};
    snapshot.bytes = recv;
    snapshot.timeMs = Utils::GetCurrentTimeMS();
    g_IOSnapshotsRecv[id] = snapshot;

    return bytesDiff;
}

long NetworkApi::GetInterfaceSend(int id) {
    if(!Utils::mapContains(g_IOSnapshotsSend, id)) {
        auto sent = GetInterfaceTotalSent(id);
        NetworkApiIOSnapshot snapshot{};
        snapshot.bytes = sent;
        snapshot.timeMs = Utils::GetCurrentTimeMS();
        g_IOSnapshotsSend[id] = snapshot;

        return 0;
    }

    auto sent = GetInterfaceTotalSent(id);

    auto lastBytes = g_IOSnapshotsSend[id];
    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastBytes.timeMs) / 1000.;
    auto bytesDiff = sent - lastBytes.bytes;

    auto mul = 1.;
    if(timeDiffS < 1)
        mul += timeDiffS;

    bytesDiff = (long)((double)bytesDiff * mul);

    NetworkApiIOSnapshot snapshot{};
    snapshot.bytes = sent;
    snapshot.timeMs = Utils::GetCurrentTimeMS();
    g_IOSnapshotsSend[id] = snapshot;

    return bytesDiff;
}

long NetworkApi::GetInterfaceTotalReceived(int id) {
    auto interfacePath = g_InterfaceMap[id];
    auto valText = IOUtils::ReadAllText(interfacePath + "/statistics/rx_bytes");
    return Utils::stringToLong(valText);
}

long NetworkApi::GetInterfaceTotalSent(int id) {
    auto interfacePath = g_InterfaceMap[id];
    auto valText = IOUtils::ReadAllText(interfacePath + "/statistics/tx_bytes");
    return Utils::stringToLong(valText);
}

std::string NetworkApi::GetInterfaceAddress(int id, NetworkInterfaceAddress addressType) {
    auto interfacePath = std::filesystem::path(g_InterfaceMap[id]);

    std::string result = "Unknown";
    switch (addressType) {
        case NetworkInterfaceAddress::IPV4: {
            struct ifreq ifr{};
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            ifr.ifr_addr.sa_family = AF_INET;

            strncpy(ifr.ifr_name, interfacePath.stem().string().c_str(), IFNAMSIZ-1);
            ioctl(fd, SIOCGIFADDR, &ifr);
            close(fd);

            auto ipStr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
            result = ipStr;
            break;
        }
        case NetworkInterfaceAddress::IPV6: {
            struct ifaddrs *ifap, *ifa;
            struct sockaddr_in6 *sa;
            char addr[INET6_ADDRSTRLEN];

            if (getifaddrs(&ifap) == -1) {
                break;
            }
            for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6) {
                    sa = (struct sockaddr_in6 *) ifa->ifa_addr;
                    getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), addr,
                                sizeof(addr), NULL, 0, NI_NUMERICHOST);
                    if(strcmp(ifa->ifa_name, interfacePath.stem().string().c_str()) == 0) {
                        result = addr;
                        result = result.substr(0, result.find('%'));
                    }
                }
            }

            freeifaddrs(ifap);
            break;
        }
        case NetworkInterfaceAddress::MAC:
            result = IOUtils::ReadAllText(interfacePath.string() + "/address");
            break;
        default:
            break;
    }

    return result;
}
// ToDo /carrier for physical connect status
long NetworkApi::GetInterfaceSpeed(int id) {
    auto interfacePath = g_InterfaceMap[id];
    auto speedText = IOUtils::ReadAllText(interfacePath + "/speed");
    if(speedText.empty())
        return 0;

    return Utils::stringToLong(speedText);
}
