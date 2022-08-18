#ifndef WSYSMON_UNITCONVERTER_H
#define WSYSMON_UNITCONVERTER_H

#include <string>

enum UnitType {
    AUTO = 0,
    KB = 1,
    MB = 2,
    GB = 3,
    TB = 4,

    AUTO_I = 5,
    KiB = 6,
    MiB = 7,
    GiB = 8,
    TiB = 9
};

enum BitUnitType {
    AUTOM = 0,
    KBIT = 1,
    MBIT = 2,
    GBIT = 3
};

struct UnitConvertResult {
    double value;
    UnitType type;
};

struct BitUnitConvertResult {
    double value;
    BitUnitType type;
};

class UnitConverter {
public:
    static std::string ConvertBytesString(long bytes, UnitType unitType = UnitType::AUTO);
    static UnitConvertResult ConvertBytes(long bytes, UnitType unitType = UnitType::AUTO);
    static UnitType GetBestUnitForBytes(long bytes, bool isBiBytes = false);

    static std::string UnitTypeToString(UnitType unitType);

    static std::string ConvertBytesToBitsString(long bytes, BitUnitType unitType = BitUnitType::AUTOM);
    static BitUnitConvertResult ConvertBytesToBits(long bytes, BitUnitType unitType = BitUnitType::AUTOM);
    static std::string BitUnitTypeToString(BitUnitType unitType);
private:
    UnitConverter() = default;
};


#endif //WSYSMON_UNITCONVERTER_H
