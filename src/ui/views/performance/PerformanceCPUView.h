#ifndef WSYSMON_PERFORMANCECPUVIEW_H
#define WSYSMON_PERFORMANCECPUVIEW_H

#include "PerformanceSubView.h"

class PerformanceCPUView : public PerformanceSubView {
public:
    PerformanceCPUView(MainWindow *window, PerformanceButton *button);

private:
    bool OnGraphClick(GdkEventButton *button);

    bool m_DisplayCoreGraphs;
    std::map<int, GraphWidget *> m_CpuGraphs;

    Gtk::Label m_FlowCpuUsage;
    Gtk::Label m_FlowCpuSpeed;
    Gtk::Label m_FlowNumProcesses;
    Gtk::Label m_FlowNumThreads;
    Gtk::Label m_FlowUptime;
    Gtk::Label m_FlowTemperature;

    Gtk::Label m_DetailMaxCpuSpeed;
    Gtk::Label m_DetailNumSockets;
    Gtk::Label m_DetailNumCores;
    Gtk::Label m_DetailNumCoresLogical;
};


#endif //WSYSMON_PERFORMANCECPUVIEW_H
