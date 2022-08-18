#ifndef WSYSMON_IOUTILS_H
#define WSYSMON_IOUTILS_H

#include <fstream>

#include "Utils.h"

class IOUtils {
public:
    static std::string ReadAllText(const std::string& path) {
        try {
            std::ifstream file;
            file.exceptions(std::fstream::failbit | std::fstream::badbit);
            file.open(path);

            std::string result((std::istreambuf_iterator<char>(file)),
                               (std::istreambuf_iterator<char>()));
            return result;
        } catch (std::exception &ex) {
            return {};
        }
    }

    static std::string ReadAllTextNullByte(const std::string& path) {
        try {
            std::ifstream file;
            file.exceptions(std::fstream::failbit | std::fstream::badbit);
            file.open(path);

            std::string result;
            std::getline(file, result, '\0');
            return result;
        } catch (std::exception &ex) {
            return {};
        }
    }

    static std::vector<std::string> ReadAllLines(const std::string& path) {
        try {
            std::vector<std::string> result;
            std::ifstream file(path);
            std::string str;
            while (std::getline(file, str)) {
                result.push_back(str);
            }
            return result;
        } catch (std::exception &ex) {
            return {};
        }
    }

private:
    IOUtils() = default;
};

#endif //WSYSMON_IOUTILS_H
