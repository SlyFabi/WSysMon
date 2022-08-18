#ifndef WSYSMON_PERFORMANCESUBVIEW_H
#define WSYSMON_PERFORMANCESUBVIEW_H

#include "../View.h"
#include "../../widgets/PerformanceButton.h"
#include "../../widgets/GraphWidget.h"
#include "../../../utils/DispatcherThread.h"
#include "../../../utils/Utils.h"

class PerformanceSubView : public View {
public:
    PerformanceSubView(MainWindow *window, PerformanceButton *button, GraphWidget *widget)
        : View(window) {
        m_Button = button;
        m_UsageGraph = widget;

        m_UsageGraph->SetSize(800, 400);
        m_UsageGraph->SetMargin(20);

        //m_UsageGraphBox.set_halign(Gtk::ALIGN_FILL);
       // m_UsageGraphBox.set_valign(Gtk::ALIGN_FILL);
        m_UsageGraphBox.set_selection_mode(Gtk::SELECTION_NONE);
        m_UsageGraphBox.set_homogeneous(true);

        m_UsageGraphBox.set_margin_top(20);
        m_UsageGraphBox.add(*m_UsageGraph);

        m_RootBox.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_RootBox.set_homogeneous(false);

        m_HeadlineBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        m_HeadlineBox.set_homogeneous(true);

        m_HeadlineDevice.set_halign(Gtk::ALIGN_END);

        Pango::FontDescription fontDesc;
        fontDesc.set_size(26 * PANGO_SCALE);
        m_HeadlineTitle.override_font(fontDesc);
        m_HeadlineTitle.set_margin_left(20);
        m_HeadlineTitle.set_margin_top(10);
        m_HeadlineTitle.set_alignment(0, 0);

        fontDesc.set_size(14 * PANGO_SCALE);
        m_HeadlineDevice.override_font(fontDesc);
        m_HeadlineDevice.set_margin_right(20);
        m_HeadlineDevice.set_margin_top(23);
        m_HeadlineDevice.set_alignment(1, 0);

        m_HeadlineBox.add(m_HeadlineTitle);
        m_HeadlineBox.add(m_HeadlineDevice);

        m_DetailBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        m_DetailBox.set_homogeneous(true);

        m_DetailFlowBox.set_valign(Gtk::ALIGN_FILL);
        m_DetailFlowBox.set_selection_mode(Gtk::SELECTION_NONE);

        m_DetailDetailBox.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_DetailDetailBox.set_homogeneous(false);
        m_DetailDetailBox.set_margin_left(20);

        m_DetailBox.add(m_DetailFlowBox);
        m_DetailBox.add(m_DetailDetailBox);

        m_RootBox.add(m_HeadlineBox);
        m_RootBox.add(m_UsageGraphBox);
        m_RootBox.add(m_DetailBox);
    }

    ~PerformanceSubView() {
        m_UpdateThread->Stop(false);
        SAFE_DELETE(m_UpdateThread);
        SAFE_DELETE(m_UsageGraph);
    }

    void OnShown() override {
    }

    void OnHidden() override {
    }

    Gtk::Widget& GetRootWidget() override {
        return m_RootBox;
    }

    void AddFlowDetail(std::string name, Gtk::Label &label) {
        auto box = new Gtk::Box();
        box->set_orientation(Gtk::ORIENTATION_VERTICAL);
        box->set_homogeneous(true);

        auto margin = 15;
        box->set_margin_top(margin);
        box->set_margin_left(margin);
        box->set_margin_right(margin);
        box->set_margin_bottom(margin);

        auto descLabel = new Gtk::Label(name);
        descLabel->set_alignment(0, 0);
        label.set_alignment(0, 0);

        Gdk::RGBA descColor;
        descColor.set_rgba(0.6, 0.6, 0.6);
        descLabel->override_color(descColor);

        Pango::FontDescription labelDesc;
        labelDesc.set_size(18 * PANGO_SCALE);
        label.override_font(labelDesc);

        box->add(*descLabel);
        box->add(label);
        m_DetailFlowBox.add(*box);
    }

    void AddDetail(std::string name, Gtk::Label &label) {
        auto box = new Gtk::Box();
        box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        box->set_homogeneous(true);

        auto margin = 5;
        box->set_margin_top(15);
        box->set_margin_left(margin);
        box->set_margin_right(20);
        box->set_margin_bottom(margin);

        auto descLabel = new Gtk::Label(name);
        descLabel->set_alignment(0, 0);
        label.set_alignment(1, 0);

        Gdk::RGBA descColor;
        descColor.set_rgba(0.6, 0.6, 0.6);
        descLabel->override_color(descColor);

        box->add(*descLabel);
        box->add(label);
        m_DetailDetailBox.add(*box);
    }

protected:
    Gtk::Box m_RootBox;

    Gtk::Box m_HeadlineBox;
    Gtk::Label m_HeadlineTitle;
    Gtk::Label m_HeadlineDevice;

    Gtk::FlowBox m_UsageGraphBox;
    GraphWidget *m_UsageGraph{};

    Gtk::Box m_DetailBox;
    Gtk::FlowBox m_DetailFlowBox;
    Gtk::Box m_DetailDetailBox;

    PerformanceButton *m_Button;

    DispatcherThread *m_UpdateThread{};
};

#endif //WSYSMON_PERFORMANCESUBVIEW_H
