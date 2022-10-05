#ifndef WSYSMON_DETAILSWINDOW_H
#define WSYSMON_DETAILSWINDOW_H

#include <gtkmm.h>

class DetailsWindow : public Gtk::Window {
public:
    explicit DetailsWindow();
    ~DetailsWindow() override;

    void Clear();
    void AddDetail(const std::string& name, const std::string& val);

private:
    Gtk::HeaderBar m_HeaderBar;
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::Grid m_DetailsGrid;

    int m_CurrentRow{};
    std::vector<Gtk::Label*> m_Labels{};
};


#endif //WSYSMON_DETAILSWINDOW_H
