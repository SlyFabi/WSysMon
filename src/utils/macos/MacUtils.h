#ifndef WSYSMON_MACUTILS_H
#define WSYSMON_MACUTILS_H

#include <vector>

class MacUtils {
public:
    static std::vector<int> GetAllPidsWithWindows();

private:
    MacUtils() = default;
};

#endif //WSYSMON_MACUTILS_H
