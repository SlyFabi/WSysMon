#ifndef WSYSMON_PERFORMANCEPSUVIEW_H
#define WSYSMON_PERFORMANCEPSUVIEW_H

#include "PerformanceSubView.h"

class PerformancePSUView : public PerformanceSubView {
public:
    explicit PerformancePSUView(MainWindow *window, PerformanceButton *button, int hwMonId);

private:
    int m_HwMonId;
    std::string m_PowerUsagePath;

    Gtk::Label m_FlowPowerUsage;

    Gtk::Label m_DetailName;
};


#endif //WSYSMON_PERFORMANCEPSUVIEW_H
