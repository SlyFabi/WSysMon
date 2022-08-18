#include "UnitConverter.h"
#include <spdlog/fmt/fmt.h>

std::string UnitConverter::ConvertBytesString(long bytes, UnitType unitType) {
    auto result = ConvertBytes(bytes, unitType);
    if(result.type == UnitType::AUTO || result.type == UnitType::AUTO_I)
        return fmt::format("{}B", bytes);

    return fmt::format("{:.2f}{}", result.value, UnitTypeToString(result.type));
}

UnitConvertResult UnitConverter::ConvertBytes(long bytes, UnitType unitType) {
    UnitConvertResult result{};
    if(unitType == UnitType::AUTO || unitType == UnitType::AUTO_I) {
        auto bytesTmp = bytes;
        auto unitTarget = (int)unitType;
        auto divisor = unitType == UnitType::AUTO ? 1000 : 1024;
        while (bytesTmp / divisor >= divisor) {
            bytesTmp /= divisor;
            unitTarget++;
        }

        unitTarget++;
        unitType = (UnitType)unitTarget;
        if(unitType == UnitType::AUTO || unitType == UnitType::AUTO_I) {
            result.value = (double)bytes;
            result.type = UnitType::AUTO;
            return result;
        }

        return ConvertBytes(bytes, unitType);
    }

    auto converted = (double)bytes;
    switch (unitType) {
        case UnitType::KB:
            converted = (double)bytes / 1000.;
            break;
        case UnitType::MB:
            converted = (double)bytes / 1000000.;
            break;
        case UnitType::GB:
            converted = (double)bytes / 1000000000.;
            break;
        case UnitType::TB:
            converted = (double)bytes / 1000000000000.;
            break;

        case UnitType::KiB:
            converted = (double)bytes / 1024.;
            break;
        case UnitType::MiB:
            converted = (double)bytes / 1024. / 1024.;
            break;
        case UnitType::GiB:
            converted = (double)bytes / 1024. / 1024. / 1024.;
            break;
        case UnitType::TiB:
            converted = (double)bytes / 1024. / 1024. / 1024. / 1024.;
            break;

        default: {
            result.value = converted;
            result.type = UnitType::AUTO;
            return result;
        }
    }

    result.value = converted;
    result.type = unitType;
    return result;
}

UnitType UnitConverter::GetBestUnitForBytes(long bytes, bool isBiBytes) {
    auto bytesTmp = bytes;
    auto unitTarget = (int)(isBiBytes ? UnitType::AUTO_I : UnitType::AUTO);
    auto divisor = isBiBytes ? 1024 : 1000;
    while (bytesTmp / divisor >= divisor) {
        bytesTmp /= divisor;
        unitTarget++;
    }

    unitTarget++;
    return (UnitType)unitTarget;
}

std::string UnitConverter::UnitTypeToString(UnitType unitType) {
    switch(unitType) {
        case UnitType::KB:
            return "KB";
        case UnitType::MB:
            return "MB";
        case UnitType::GB:
            return "GB";
        case UnitType::TB:
            return "TB";

        case UnitType::KiB:
            return "KiB";
        case UnitType::MiB:
            return "MiB";
        case UnitType::GiB:
            return "GiB";
        case UnitType::TiB:
            return "TiB";
        default:
            return "";
    }
}

std::string UnitConverter::ConvertBytesToBitsString(long bytes, BitUnitType unitType) {
    auto result = ConvertBytesToBits(bytes, unitType);
    if(result.type == BitUnitType::AUTOM)
        return fmt::format("{}b", bytes);

    return fmt::format("{:.2f}{}", result.value, BitUnitTypeToString(result.type));
}

BitUnitConvertResult UnitConverter::ConvertBytesToBits(long bytes, BitUnitType unitType) {
    BitUnitConvertResult result{};
    auto bits = bytes * 8;
    if(unitType == BitUnitType::AUTOM) {
        auto bitsTmp = bits;
        auto unitTarget = (int)unitType;
        auto divisor = 1000;
        while (bitsTmp / divisor >= divisor) {
            bitsTmp /= divisor;
            unitTarget++;
        }

        unitTarget++;
        unitType = (BitUnitType)unitTarget;
        if(unitType == BitUnitType::AUTOM) {
            result.value = (double)bits;
            result.type = BitUnitType::AUTOM;
            return result;
        }

        return ConvertBytesToBits(bytes, unitType);
    }

    auto converted = (double)bits;
    switch (unitType) {
        case BitUnitType::KBIT:
            converted = (double)bits / 1000.;
            break;
        case BitUnitType::MBIT:
            converted = (double)bits / 1000000.;
            break;
        case BitUnitType::GBIT:
            converted = (double)bits / 1000000000.;
            break;

        default: {
            result.value = converted;
            result.type = BitUnitType::AUTOM;
            return result;
        }
    }

    result.value = converted;
    result.type = unitType;
    return result;
}

std::string UnitConverter::BitUnitTypeToString(BitUnitType unitType) {
    switch(unitType) {
        case BitUnitType::KBIT:
            return "Kbit";
        case BitUnitType::MBIT:
            return "Mbit";
        case BitUnitType::GBIT:
            return "Gbit";
        default:
            return "";
    }
}
