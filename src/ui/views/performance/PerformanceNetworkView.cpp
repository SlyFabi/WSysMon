#include <spdlog/fmt/fmt.h>
#include "PerformanceNetworkView.h"
#include "../../../api/NetworkApi.h"
#include "../../../utils/UnitConverter.h"

PerformanceNetworkView::PerformanceNetworkView(MainWindow *window, PerformanceButton *button, int id)
    : PerformanceSubView(window, button, new GraphWidget(0, 100, 60)) {
    Gdk::RGBA color, secColor;
    color.set_rgba(177 / 256., 134 / 256., 24 / 256., 1.);
    secColor.set_rgba(127 / 256., 84 / 256., 14 / 256., 1.);

    button->GetGraph()->SetAxisColor(color);
    button->GetGraph()->SetSecondaryColor(secColor);
    m_UsageGraph->SetAxisColor(color);
    m_UsageGraph->SetAxisUnit("Mbit/s");

    m_InterfaceId = id;
    auto name = NetworkApi::GetInterfaceName(m_InterfaceId);
    m_HeadlineTitle.set_text("Network");
    m_HeadlineDevice.set_text(name);

    m_Button->SetTitle(name);
    m_Button->SetInfoText("Recv: 0Kbit/s");
    m_Button->SetInfoText2("Send: 0Kbit/s");

    AddFlowDetail("Receive", m_FlowReceive);
    AddFlowDetail("Total received", m_FlowTotalReceived);
    AddFlowDetail("Send", m_FlowSend);
    AddFlowDetail("Total sent", m_FlowTotalSent);

    AddDetail("Speed:", m_DetailSpeed);
    AddDetail("IPv4 Address:", m_DetailIpV4);
    AddDetail("IPv6 Address:", m_DetailIpV6);
    AddDetail("Mac Address:", m_DetailMacAddress);

    auto interfaceSpeed = NetworkApi::GetInterfaceSpeed(m_InterfaceId);

    m_DetailSpeed.set_text(fmt::format("{} Mbit/s", interfaceSpeed));
    m_DetailIpV4.set_text(NetworkApi::GetInterfaceAddress(m_InterfaceId, NetworkInterfaceAddress::IPV4));
    m_DetailIpV6.set_text(NetworkApi::GetInterfaceAddress(m_InterfaceId, NetworkInterfaceAddress::IPV6));
    m_DetailMacAddress.set_text(NetworkApi::GetInterfaceAddress(m_InterfaceId, NetworkInterfaceAddress::MAC));

    m_UsageGraph->SetLimits(0, 1);
    m_Button->GetGraph()->SetLimits(0, 1);
    m_UpdateThread = new DispatcherThread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_UpdateThread->Dispatch();
        }, [this]() {
            auto receive = NetworkApi::GetInterfaceReceive(m_InterfaceId);
            auto send = NetworkApi::GetInterfaceSend(m_InterfaceId);
            auto totalReceived = NetworkApi::GetInterfaceTotalReceived(m_InterfaceId);
            auto totalSent = NetworkApi::GetInterfaceTotalSent(m_InterfaceId);

            m_FlowReceive.set_text(fmt::format("{}/s", UnitConverter::ConvertBytesString(receive)));
            m_FlowSend.set_text(fmt::format("{}/s", UnitConverter::ConvertBytesString(send)));
            m_FlowTotalReceived.set_text(UnitConverter::ConvertBytesString(totalReceived));
            m_FlowTotalSent.set_text(UnitConverter::ConvertBytesString(totalSent));

            auto recvConv = UnitConverter::ConvertBytesToBits(receive);
            auto sentConv = UnitConverter::ConvertBytesToBits(send);
            m_Button->SetInfoText(fmt::format("Recv: {:.0f}{}/s", recvConv.value, UnitConverter::BitUnitTypeToString(recvConv.type)));
            m_Button->SetInfoText2(fmt::format("Send: {:.0f}{}/s", sentConv.value, UnitConverter::BitUnitTypeToString(sentConv.type)));

            auto recvPoint = UnitConverter::ConvertBytesToBits(receive, BitUnitType::MBIT).value;
            auto sendPoint = UnitConverter::ConvertBytesToBits(send, BitUnitType::MBIT).value;
            m_UsageGraph->AddPoint(recvPoint);
            m_UsageGraph->AddPoint2(sendPoint);

            m_Button->AddGraphPoint(recvPoint);
            m_Button->AddGraphPoint2(sendPoint);
    });
    m_UpdateThread->Start();
}
