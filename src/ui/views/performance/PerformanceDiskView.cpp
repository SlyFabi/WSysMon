#include <spdlog/fmt/fmt.h>
#include <filesystem>

#include "PerformanceDiskView.h"
#include "../../../api/DiskApi.h"
#include "../../../utils/UnitConverter.h"

PerformanceDiskView::PerformanceDiskView(MainWindow *window, PerformanceButton *button, int id)
        : PerformanceSubView(window, button, new GraphWidget(0, 100, 60)) {
    Gdk::RGBA color;
    color.set_rgba(64 / 256., 102 / 256., 62 / 256., 1.);
    button->GetGraph()->SetAxisColor(color);
    m_UsageGraph->SetAxisColor(color);

    m_InterfaceId = id;
    auto title = fmt::format("Disk {}", m_InterfaceId);
    auto diskName = DiskApi::GetDiskName(m_InterfaceId);
    m_HeadlineTitle.set_text(title);
    m_HeadlineDevice.set_text(diskName);

    m_Button->SetTitle(title);
    m_Button->SetInfoText(std::filesystem::path(DiskApi::GetDiskPath(m_InterfaceId)).stem().string());
    m_Button->SetInfoText2("0%");

    AddFlowDetail("Read Speed", m_FlowRead);
    AddFlowDetail("Write Speed", m_FlowWritten);
    AddFlowDetail("Total read", m_FlowTotalRead);
    AddFlowDetail("Total written", m_FlowTotalWritten);

    AddDetail("Type:", m_DetailType);
    AddDetail("Path:", m_DetailPath);
    AddDetail("Total space:", m_DetailTotalSpace);

    switch (DiskApi::GetDiskType(m_InterfaceId)) {
        case HDD:
            m_DetailType.set_text("HDD");
            break;
        case SSD:
            m_DetailType.set_text("SSD");
            break;
        case NVME:
            m_DetailType.set_text("NVME");
            break;
        case RAID:
            m_DetailType.set_text("RAID");
            break;
        default:
            m_DetailType.set_text("Unknown");
            break;
    }
    m_DetailPath.set_text(DiskApi::GetDiskPath(m_InterfaceId));
    m_DetailTotalSpace.set_text(UnitConverter::ConvertBytesString(DiskApi::GetDiskTotalSpace(m_InterfaceId)));

    m_UpdateThread = new DispatcherThread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_UpdateThread->Dispatch();
    }, [this]() {
        auto read = DiskApi::GetDiskRead(m_InterfaceId);
        auto written = DiskApi::GetDiskWrite(m_InterfaceId);
        auto totalRead = DiskApi::GetDiskTotalRead(m_InterfaceId);
        auto totalWritten = DiskApi::GetDiskTotalWritten(m_InterfaceId);

        m_FlowRead.set_text(UnitConverter::ConvertBytesString(read));
        m_FlowWritten.set_text(UnitConverter::ConvertBytesString(written));
        m_FlowTotalRead.set_text(UnitConverter::ConvertBytesString(totalRead));
        m_FlowTotalWritten.set_text(UnitConverter::ConvertBytesString(totalWritten));

        auto diskUsage = DiskApi::GetDiskUsage(m_InterfaceId);
        m_UsageGraph->AddPoint(diskUsage);
        m_Button->AddGraphPoint(diskUsage);
        m_Button->SetInfoText2(fmt::format("{:.0f}%", diskUsage));
    });
    m_UpdateThread->Start();
}
