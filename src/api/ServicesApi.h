#ifndef WSYSMON_SERVICESAPI_H
#define WSYSMON_SERVICESAPI_H

#include "../utils/Utils.h"

struct ServiceInfo {
    std::string name;
    std::string status;
};

class ServicesApi {
public:
    static std::vector<ServiceInfo> GetAllServices();

private:
    ServicesApi() = default;
};

#endif //WSYSMON_SERVICESAPI_H
