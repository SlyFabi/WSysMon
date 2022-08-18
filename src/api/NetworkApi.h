#ifndef WSYSMON_NETWORKAPI_H
#define WSYSMON_NETWORKAPI_H

#include <string>

enum NetworkInterfaceAddress {
    IPV4 = 0,
    IPV6 = 1,
    MAC = 2
};

class NetworkApi {
public:
    static int GetNumInterfaces();
    static std::string GetInterfaceName(int id);

    static long GetInterfaceReceive(int id);
    static long GetInterfaceSend(int id);

    static long GetInterfaceTotalReceived(int id);
    static long GetInterfaceTotalSent(int id);

    static std::string GetInterfaceAddress(int id, NetworkInterfaceAddress addressType);

    static long GetInterfaceSpeed(int id);

private:
    NetworkApi() = default;
};


#endif //WSYSMON_NETWORKAPI_H
