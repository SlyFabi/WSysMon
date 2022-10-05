#include "DetailsWindow.h"

DetailsWindow::DetailsWindow() {
    set_title("WSysMon");
    set_default_size(400, 250);
    set_position(Gtk::WIN_POS_CENTER);
    set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);

    // Header Bar
    m_HeaderBar.set_show_close_button(true);
    set_titlebar(m_HeaderBar);

    // Root
    m_DetailsGrid.set_column_spacing(20);
    m_DetailsGrid.set_row_spacing(5);
    m_DetailsGrid.property_margin() = 10;

    m_ScrolledWindow.add(m_DetailsGrid);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.property_vexpand() = true;

    add(m_ScrolledWindow);
    show_all_children();
}

DetailsWindow::~DetailsWindow() {
    Clear();
}

void DetailsWindow::Clear() {
    for(auto child : m_DetailsGrid.get_children())
        m_DetailsGrid.remove(*child);
    for(auto lbl : m_Labels)
        delete lbl;

    m_Labels.clear();
    m_CurrentRow = 0;
}

void DetailsWindow::AddDetail(const std::string& name, const std::string& val) {
    auto label1 = new Gtk::Label(name);
    auto label2 = new Gtk::Label(val);
    label1->set_xalign(0);
    label2->set_xalign(0);
    label1->set_selectable(true);
    label2->set_selectable(true);

    m_DetailsGrid.attach(*label1, 0, m_CurrentRow, 1, 1);
    m_DetailsGrid.attach(*label2, 1, m_CurrentRow, 1, 1);
    m_CurrentRow += 1;

    m_Labels.emplace_back(label1);
    m_Labels.emplace_back(label2);
}
