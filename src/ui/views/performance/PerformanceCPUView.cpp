#include "PerformanceCPUView.h"
#include "../../../api/SystemInfoApi.h"
#include "../../../api/ProcessesApi.h"

#include <random>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

PerformanceCPUView::PerformanceCPUView(MainWindow *window, PerformanceButton *button)
    : PerformanceSubView(window, button, new GraphWidget(0, 100, 60)) {
    m_DisplayCoreGraphs = false;
    m_HeadlineTitle.set_text("CPU");
    m_HeadlineDevice.set_text(SystemInfoApi::GetCPUName());

    AddFlowDetail("Usage", m_FlowCpuUsage);
    AddFlowDetail("Speed", m_FlowCpuSpeed);
    AddFlowDetail("Processes", m_FlowNumProcesses);
    AddFlowDetail("Threads", m_FlowNumThreads);
    AddFlowDetail("Uptime", m_FlowUptime);

    auto tempEnabled = SystemInfoApi::GetCPUTemperature() > 0;
    if(tempEnabled)
        AddFlowDetail("Temperature", m_FlowTemperature);

    AddDetail("Maximum CPU speed:", m_DetailMaxCpuSpeed);
    AddDetail("Sockets:", m_DetailNumSockets);
    AddDetail("Cores:", m_DetailNumCores);
    AddDetail("Logical processors:", m_DetailNumCoresLogical);

    m_DetailMaxCpuSpeed.set_text(fmt::format("{:.2f}GHz", (double)SystemInfoApi::GetMaxCPUClock() / 1000000000.));
    m_DetailNumCores.set_text(std::to_string(SystemInfoApi::GetNumCPUs()));
    m_DetailNumCoresLogical.set_text(std::to_string(SystemInfoApi::GetNumCPUsLogical()));
    m_DetailNumSockets.set_text(std::to_string(SystemInfoApi::GetSocketCount()));

    m_UsageGraphBox.signal_button_press_event().connect(sigc::mem_fun(*this, &PerformanceCPUView::OnGraphClick), false);

    for(int i = 0; i < SystemInfoApi::GetNumCPUsLogical(); i++) {
        auto graph = new GraphWidget(0, 100, 30);
        graph->SetSize(140, 180);
        graph->SetMargin(10, 10);
        graph->SetDrawAxisText(false);
        graph->signal_button_press_event().connect(sigc::mem_fun(*this, &PerformanceCPUView::OnGraphClick));
        m_CpuGraphs[i] = graph;
    }

    m_UpdateThread = new DispatcherThread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_UpdateThread->Dispatch();
        }, [this, tempEnabled]() {
            auto cpuUsageTotal = SystemInfoApi::GetCPUUsagePercent();
            double cpuClockGhz = (double)SystemInfoApi::GetCPUClock() / 1000000000.;
            m_Button->SetInfoText(fmt::format("{:.0f}%, {:.2f}GHz", cpuUsageTotal, cpuClockGhz));

            m_FlowCpuUsage.set_text(fmt::format("{:.2f}%", cpuUsageTotal));
            m_FlowCpuSpeed.set_text(fmt::format("{:.2f}GHz", cpuClockGhz));
            m_FlowNumProcesses.set_text(std::to_string(ProcessesApi::GetNumProcesses()));
            m_FlowNumThreads.set_text(std::to_string(ProcessesApi::GetNumThreads()));

            if(tempEnabled) {
                auto tempStr = fmt::format("{:.0f}°C", SystemInfoApi::GetCPUTemperature());
                m_FlowTemperature.set_text(tempStr);
                m_Button->SetInfoText2(tempStr);
            }

            auto uptimeMs = SystemInfoApi::GetUptimeMS();
            auto time = std::chrono::milliseconds(uptimeMs);
            m_FlowUptime.set_text(Utils::formatDuration<std::chrono::milliseconds,
                    std::chrono::hours,
                    std::chrono::minutes,
                    std::chrono::seconds>(time));

            m_Button->AddGraphPoint(cpuUsageTotal);

            for(int i = 0; i < SystemInfoApi::GetNumCPUsLogical(); i++) {
                auto cpuUsageCore = SystemInfoApi::GetCPUUsagePercent(i);
                m_CpuGraphs[i]->AddPoint(cpuUsageCore);
            }
            m_UsageGraph->AddPoint(cpuUsageTotal);
    });
    m_UpdateThread->Start();
}

bool PerformanceCPUView::OnGraphClick(GdkEventButton *button) {
    if(button->type == GdkEventType::GDK_BUTTON_PRESS && button->button == 3) {
        if(!m_DisplayCoreGraphs) {
            m_UsageGraphBox.remove(*m_UsageGraph);
            for(int i = 0; i < SystemInfoApi::GetNumCPUsLogical(); i++) {
                m_UsageGraphBox.add(*m_CpuGraphs[i]);
            }
            spdlog::debug("Core graph enabled.");
        } else {
            for(int i = 0; i < SystemInfoApi::GetNumCPUsLogical(); i++) {
                m_UsageGraphBox.remove(*m_CpuGraphs[i]);
            }
            m_UsageGraphBox.add(*m_UsageGraph);
            spdlog::debug("Core graph disabled.");
        }

        m_UsageGraphBox.show_all();
        m_RootBox.show_all();
        m_DisplayCoreGraphs = !m_DisplayCoreGraphs;
    }

    return true;
}
