#include "PerformancePSUView.h"
#include "../../../api/HwMonApi.h"
#include "../../../utils/IOUtils.h"

#define MW_TO_W 1000000

PerformancePSUView::PerformancePSUView(MainWindow *window, PerformanceButton *button, int hwMonId)
        : PerformanceSubView(window, button, new GraphWidget(0, 100, 60)) {
    m_HwMonId = hwMonId;
    m_PowerUsagePath = HwMonApi::FindInputPath(m_HwMonId, "power total");

    Gdk::RGBA graphColor;
    graphColor.set_rgba(227 / 256., 245 / 256., 0., 1.);

    button->GetGraph()->SetAxisColor(graphColor);
    m_UsageGraph->SetAxisColor(graphColor);
    m_UsageGraph->SetAxisUnit("Watts");

    m_UsageGraph->SetLimits(0, 500);
    m_Button->GetGraph()->SetLimits(0, 500);

    button->SetTitle("PSU");
    button->SetInfoText("0W");

    m_HeadlineTitle.set_text("PSU");

    AddFlowDetail("Power Usage", m_FlowPowerUsage);
    m_FlowPowerUsage.set_text("0W");

    AddDetail("Name", m_DetailName);
    m_DetailName.set_text(HwMonApi::GetDeviceName(m_HwMonId));

    m_UpdateThread = new DispatcherThread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_UpdateThread->Dispatch();
    }, [this]() {
        auto powerUsage = Utils::stringToLong(IOUtils::ReadAllText(m_PowerUsagePath));
        powerUsage /= MW_TO_W;

        m_FlowPowerUsage.set_text(fmt::format("{}W", powerUsage));
        m_UsageGraph->AddPoint((double)powerUsage);

        m_Button->SetInfoText(fmt::format("{}W", powerUsage));
        m_Button->AddGraphPoint((double)powerUsage);
    });
    m_UpdateThread->Start();
}
