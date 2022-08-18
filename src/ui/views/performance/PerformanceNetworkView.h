#ifndef WSYSMON_PERFORMANCENETWORKVIEW_H
#define WSYSMON_PERFORMANCENETWORKVIEW_H

#include "PerformanceSubView.h"

class PerformanceNetworkView : public PerformanceSubView {
public:
    PerformanceNetworkView(MainWindow *window, PerformanceButton *button, int id);

private:
    int m_InterfaceId;

    Gtk::Label m_FlowSend;
    Gtk::Label m_FlowReceive;
    Gtk::Label m_FlowTotalSent;
    Gtk::Label m_FlowTotalReceived;

    Gtk::Label m_DetailSpeed;
    Gtk::Label m_DetailIpV4;
    Gtk::Label m_DetailIpV6;
    Gtk::Label m_DetailMacAddress;
};


#endif //WSYSMON_PERFORMANCENETWORKVIEW_H
