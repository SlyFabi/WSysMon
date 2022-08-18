#ifndef WSYSMON_PERFORMANCEBUTTON_H
#define WSYSMON_PERFORMANCEBUTTON_H

#include <gtkmm.h>
#include "GraphWidget.h"

class PerformanceButton : public Gtk::Widget {
public:
    explicit PerformanceButton(int viewId);
    ~PerformanceButton() override;

    void AddGraphPoint(double val);
    void AddGraphPoint2(double val);

    GraphWidget *GetGraph();
    int GetViewId() const;

    sigc::signal<void(PerformanceButton *)> signal_click();

    void SetTitle(const std::string& title);
    void SetInfoText(const std::string& infoText);
    void SetInfoText2(const std::string& infoText);

    void set_selected(bool isSelected);
    void set_other_buttons(const std::vector<PerformanceButton *>& buttons);
protected:
    bool on_button_press_event(GdkEventButton* button_event) override;

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gtk::Allocation& allocation) override;
    void on_map() override;
    void on_unmap() override;
    void on_realize() override;
    void on_unrealize() override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
    Glib::RefPtr<Gtk::CssProvider> m_refStyleProvider;

private:
    std::string m_Title;
    std::string m_InfoText;
    std::string m_InfoText2;

    int m_ViewId;
    bool m_IsSelected{};
    sigc::signal<void(PerformanceButton *)> m_SignalClick;

    std::vector<PerformanceButton *> m_OtherButtons;

    GraphWidget *m_Graph;
};


#endif //WSYSMON_PERFORMANCEBUTTON_H
