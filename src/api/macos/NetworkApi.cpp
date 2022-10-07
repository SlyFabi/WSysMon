#include "../NetworkApi.h"

int NetworkApi::GetNumInterfaces() {
    return 0;
}

std::string NetworkApi::GetInterfaceName(int id) {
    return {};
}

long NetworkApi::GetInterfaceReceive(int id) {
    return 0;
}

long NetworkApi::GetInterfaceSend(int id) {
    return 0;
}

long NetworkApi::GetInterfaceTotalReceived(int id) {
    return 0;
}

long NetworkApi::GetInterfaceTotalSent(int id) {
    return 0;
}

std::string NetworkApi::GetInterfaceAddress(int id, NetworkInterfaceAddress addressType) {
    return {};
}

long NetworkApi::GetInterfaceSpeed(int id) {
    return 0;
}