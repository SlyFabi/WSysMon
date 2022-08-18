#include "PerformanceView.h"
#include "../../api/GPUApi.h"
#include "../../api/DiskApi.h"
#include "../../api/NetworkApi.h"
#include "performance/PerformanceNetworkView.h"
#include "performance/PerformanceDiskView.h"

PerformanceView::PerformanceView(MainWindow *window)
    : View(window) {
    m_SplitPanel.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_ButtonBox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_ButtonBox.set_homogeneous(false);

    m_ButtonScroll.set_min_content_width(300);
    m_ButtonScroll.add(m_ButtonBox);

    m_SplitPanel.add1(m_ButtonScroll);
    m_SplitPanel.child_property_shrink(m_ButtonScroll) = false;

    // CPU
    m_CpuButton = CreateButton();
    m_CpuButton->SetTitle("CPU");
    m_CpuButton->SetInfoText("0%, 0.00GHz");
    m_CpuView = new PerformanceCPUView(window, m_CpuButton);
    CreateView(m_CpuButton, m_CpuView);

    // RAM
    m_RamButton = CreateButton();
    m_RamButton->SetTitle("RAM");
    m_RamButton->SetInfoText("0.0/0 GB (0%)");
    m_RamView = new PerformanceRAMView(window, m_RamButton);
    CreateView(m_RamButton, m_RamView);

    // Disks
    int numDisks = DiskApi::GetNumDisks();
    for(int i = 0; i < numDisks; i++) {
        auto button = CreateButton();
        auto view = new PerformanceDiskView(window, button, i);
        CreateView(button, view);
    }

    // Network interfaces
    int numNetInt = NetworkApi::GetNumInterfaces();
    for(int i = 0; i < numNetInt; i++) {
        auto button = CreateButton();
        auto view = new PerformanceNetworkView(window, button, i);
        CreateView(button, view);
    }

    // GPUs
    int numGpus = GPUApi::GetNumGPUs();
    for(int i = 0; i < numGpus; i++) {
        auto button = CreateButton();
        auto view = new PerformanceGPUView(window, button, i);
        CreateView(button, view);
    }

    for(auto button : m_Buttons) {
        button->set_other_buttons(m_Buttons);
    }

    SetView(0);
    m_CpuButton->set_selected(true);
}

PerformanceView::~PerformanceView() {
    for(auto btn : m_Buttons) {
        delete btn;
    }
    m_Buttons.clear();

    std::vector<std::thread> deleteThreads;
    for(auto pair : m_Views) {
        delete pair.second;
    }
    m_Views.clear();

    m_CpuView = nullptr;
    m_RamView = nullptr;
    m_CpuButton = nullptr;
    m_RamButton = nullptr;
}

void PerformanceView::OnShown() {
}

void PerformanceView::OnHidden() {
}

Gtk::Widget &PerformanceView::GetRootWidget() {
    return m_SplitPanel;
}

void PerformanceView::SetView(int viewId) {
    if(m_SplitPanel.get_child2() != nullptr) {
        m_SplitPanel.remove(*m_SplitPanel.get_child2());
    }

    auto view = m_Views[viewId];
    m_SplitPanel.add2(view->GetRootWidget());
    m_SplitPanel.child_property_shrink(view->GetRootWidget()) = false;
}

PerformanceButton *PerformanceView::CreateButton() {
    auto button = new PerformanceButton(NextViewId());
    button->signal_click().connect(sigc::mem_fun(*this, &PerformanceView::OnButtonClick));
    m_ButtonBox.pack_start(*button, false, false, 10);
    m_Buttons.push_back(button);
    return button;
}

void PerformanceView::CreateView(PerformanceButton *button, PerformanceSubView *view) {
    auto viewId = button->GetViewId();
    m_Views.insert(std::make_pair(viewId, view));
}

void PerformanceView::OnButtonClick(PerformanceButton *button) {
    auto viewId = button->GetViewId();
    SetView(viewId);
    m_SplitPanel.show_all();
}

int PerformanceView::NextViewId() {
    return m_NextViewId++;
}
