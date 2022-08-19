#ifndef WSYSMON_PERFORMANCEGPUVIEW_H
#define WSYSMON_PERFORMANCEGPUVIEW_H

#include "PerformanceSubView.h"

class PerformanceGPUView : public PerformanceSubView {
public:
    explicit PerformanceGPUView(MainWindow *window, PerformanceButton *button, int gpuId);

private:
    int m_GpuId;

    Gtk::Label m_FlowGpuUsage;
    Gtk::Label m_FlowGpuClock;
    Gtk::Label m_FlowGpuMemory;
    Gtk::Label m_FlowGpuTemperature;

    Gtk::Label m_DetailDriver;
};


#endif //WSYSMON_PERFORMANCEGPUVIEW_H
