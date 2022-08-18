#include <spdlog/fmt/fmt.h>
#include "PerformanceRAMView.h"
#include "../../../api/SystemInfoApi.h"
#include "../../../utils/UnitConverter.h"

PerformanceRAMView::PerformanceRAMView(MainWindow *window, PerformanceButton *button)
    : PerformanceSubView(window, button, new GraphWidget(0, 100, 60)) {
    Gdk::RGBA ramColor;
    ramColor.set_rgba(137 / 256., 97 / 256., 153 / 256., 1.);
    button->GetGraph()->SetAxisColor(ramColor);

    m_HeadlineTitle.set_text("RAM");
    m_HeadlineDevice.set_text("");

    m_UsageGraph->SetAxisColor(ramColor);

    AddFlowDetail("Usage", m_FlowMemoryUsage);
    AddFlowDetail("Used", m_FlowUsedMemory);
    AddFlowDetail("Available", m_FlowAvailableMemory);
    AddFlowDetail("Total", m_FlowTotalMemory);

    m_UpdateThread = new DispatcherThread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_UpdateThread->Dispatch();
        }, [this]() {
            auto totalRam = SystemInfoApi::GetTotalRam();
            auto usedRam = SystemInfoApi::GetUsedRam();
            auto availableRam = SystemInfoApi::GetFreeRam();
            auto ramUsagePercent = SystemInfoApi::GetRamUsagePercent() * 100.;

            auto ramUnit = UnitConverter::GetBestUnitForBytes(totalRam);
            m_Button->SetInfoText(fmt::format("{:.1f}/{:.0f} {} ({:.0f} %)",
                                              UnitConverter::ConvertBytes(usedRam, ramUnit).value,
                                              UnitConverter::ConvertBytes(totalRam, ramUnit).value,
                                              UnitConverter::UnitTypeToString(ramUnit),
                                              ramUsagePercent));

            m_FlowMemoryUsage.set_text(fmt::format("{:.0f}%", ramUsagePercent));
            m_FlowUsedMemory.set_text(UnitConverter::ConvertBytesString(usedRam));
            m_FlowAvailableMemory.set_text(UnitConverter::ConvertBytesString(availableRam));
            m_FlowTotalMemory.set_text(UnitConverter::ConvertBytesString(totalRam));

            m_UsageGraph->AddPoint(ramUsagePercent);
            m_Button->AddGraphPoint(ramUsagePercent);
    });
    m_UpdateThread->Start();
}
