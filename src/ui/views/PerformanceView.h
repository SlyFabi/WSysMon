#ifndef WSYSMON_PERFORMANCEVIEW_H
#define WSYSMON_PERFORMANCEVIEW_H

#include "View.h"
#include "../widgets/PerformanceButton.h"
#include "performance/PerformanceCPUView.h"
#include "performance/PerformanceRAMView.h"
#include "performance/PerformanceGPUView.h"

#include <map>

class PerformanceView : public View {
public:
    explicit PerformanceView(MainWindow *window);
    ~PerformanceView();

    void OnShown() override;
    void OnHidden() override;

    Gtk::Widget& GetRootWidget() override;

    void SetView(int viewId);

private:
    PerformanceButton *CreateButton();
    void CreateView(PerformanceButton *button, PerformanceSubView *view);
    void OnButtonClick(PerformanceButton *button);

    int NextViewId();

    Gtk::Paned m_SplitPanel;
    Gtk::ScrolledWindow m_ButtonScroll;
    Gtk::Box m_ButtonBox;

    int m_NextViewId{};
    std::vector<PerformanceButton *> m_Buttons;
    std::map<int, PerformanceSubView *> m_Views;

    PerformanceButton *m_CpuButton;
    PerformanceButton *m_RamButton;
    std::vector<PerformanceButton *> m_GpuButtons;

    PerformanceCPUView *m_CpuView;
    PerformanceRAMView *m_RamView;
    std::vector<PerformanceGPUView *> m_GpuViews;
};


#endif //WSYSMON_PERFORMANCEVIEW_H
