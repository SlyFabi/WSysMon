#ifndef WSYSMON_UTILS_H
#define WSYSMON_UTILS_H

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <regex>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <spdlog/spdlog.h>

#define SAFE_DELETE(x) if(x != nullptr) { delete x; x = nullptr; }

class Utils {
public:
    static long GetCurrentTimeMS() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    template<class DurationIn, class FirstDuration, class...RestDurations>
    static std::string formatDuration(DurationIn d) {
        auto val = std::chrono::duration_cast<FirstDuration>(d);
        auto out = fmt::format("{:#02d}", val.count());
        if constexpr(sizeof...(RestDurations) > 0) {
            out += ":" + formatDuration<DurationIn, RestDurations...>(d - val);
        }
        return out;
    }

    template<typename T>
    static bool vectorContains(const std::vector<T>& vec, T val) {
        auto found = std::find_if(vec.begin(), vec.end(),[&](T type) -> bool { return type == val; });
        return found != std::end(vec);
    }

    template<typename T, typename K>
    static inline bool mapContains(const std::map<T, K>& map, T key) {
        return map.count(key);
    }

    static bool stringStartsWith(const std::string& str, const std::string& start) {
        return str.rfind(start, 0) == 0;
    }

    static bool stringEndsWith(const std::string& str, const std::string& end) {
        if (end.size() > str.size()) return false;
        return std::equal(end.rbegin(), end.rend(), str.rbegin());
    }

    static std::vector<std::string> stringSplit(std::string str, const std::string& splitStr) {
        std::vector<std::string> result;
        size_t pos;
        while ((pos = str.find(splitStr)) != std::string::npos) {
            auto token = str.substr(0, pos);
            result.push_back(token);
            str.erase(0, pos + splitStr.length());
        }

        return result;
    }

    static std::vector<std::string> stringSplitRegex(const std::string &str, const std::string& regex) {
        std::vector<std::string> elems;

        std::regex rgx(regex);
        std::sregex_token_iterator iter(str.begin(), str.end(), rgx, -1);
        std::sregex_token_iterator end;

        while (iter != end)  {
            elems.push_back(*iter);
            ++iter;
        }

        return elems;
    }

    static inline std::string stringTrim(std::string str) {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), str.end());
        return str;
    }

    static std::string stringReplace(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    static long stringToLong(const std::string& str) {
        errno = 0;
        char *end;
        auto i = std::strtol(str.c_str(), &end, 10);
        if(errno == ERANGE)
            return -1;
        return i;
    }

    static double stringToDouble(const std::string& str) {
        errno = 0;
        char *end;
        auto i = std::strtod(str.c_str(), &end);
        if(errno == ERANGE)
            return -1;
        return i;
    }

    static std::string stringToLower(const std::string& str) {
        auto copy = std::string(str);
        std::transform(copy.begin(), copy.end(), copy.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return copy;
    }

private:
    Utils() = default;
};

#endif //WSYSMON_UTILS_H
