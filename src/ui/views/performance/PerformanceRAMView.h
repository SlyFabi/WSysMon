#ifndef WSYSMON_PERFORMANCERAMVIEW_H
#define WSYSMON_PERFORMANCERAMVIEW_H

#include "PerformanceSubView.h"

class PerformanceRAMView : public PerformanceSubView {
public:
    explicit PerformanceRAMView(MainWindow *window, PerformanceButton *button);

private:
    Gtk::Label m_FlowMemoryUsage;
    Gtk::Label m_FlowUsedMemory;
    Gtk::Label m_FlowAvailableMemory;
    Gtk::Label m_FlowTotalMemory;
};

#endif //WSYSMON_PERFORMANCERAMVIEW_H
