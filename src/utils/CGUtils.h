#ifndef WSYSMON_CGUTILS_H
#define WSYSMON_CGUTILS_H

#include "Utils.h"

class CGUtils {
public:
    static std::vector<int> GetAllPidsWithWindows();

private:
    CGUtils() = default;
};


#endif //WSYSMON_CGUTILS_H
