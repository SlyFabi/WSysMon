#ifndef WSYSMON_HWMONAPI_H
#define WSYSMON_HWMONAPI_H

#include "../utils/Utils.h"

class HwMonApi {
public:
    static int GetNumDevices();

    static std::string GetDeviceName(int id);
    static std::string FindInputPath(int id, const std::string& inputName);

private:

    HwMonApi() = default;
};

#endif //WSYSMON_HWMONAPI_H
