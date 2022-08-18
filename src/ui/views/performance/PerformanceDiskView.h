#ifndef WSYSMON_PERFORMANCEDISKVIEW_H
#define WSYSMON_PERFORMANCEDISKVIEW_H

#include "PerformanceSubView.h"

class PerformanceDiskView : public PerformanceSubView {
public:
    PerformanceDiskView(MainWindow *window, PerformanceButton *button, int id);

private:
    int m_InterfaceId;

    Gtk::Label m_FlowRead;
    Gtk::Label m_FlowWritten;
    Gtk::Label m_FlowTotalRead;
    Gtk::Label m_FlowTotalWritten;

    Gtk::Label m_DetailType;
    Gtk::Label m_DetailPath;
    Gtk::Label m_DetailTotalSpace;
};


#endif //WSYSMON_PERFORMANCEDISKVIEW_H
