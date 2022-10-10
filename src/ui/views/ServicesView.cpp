#include "ServicesView.h"

#include "../../api/ServicesApi.h"

ServicesView::ServicesView(MainWindow *window) : View(window) {
    // Tree view
    m_ServicesTreeModel = Gtk::TreeStore::create(m_ServicesTreeModelColumns);
    m_ServicesTreeView.set_model(m_ServicesTreeModel);

    m_ServicesTreeView.set_activate_on_single_click(true);
    m_ServicesTreeView.signal_cursor_changed().connect_notify(sigc::mem_fun(*this, &ServicesView::OnRowSelected), false);
    m_ServicesTreeView.signal_button_release_event().connect_notify(sigc::mem_fun(*this, &ServicesView::OnRowClick), false);
    m_ServicesTreeModelColumns.CreateColumns(m_ServicesTreeView);

    // Column size
    for(int i = 0; i < m_ServicesTreeView.get_n_columns(); i++) {
        auto col = m_ServicesTreeView.get_column(i);
        col->set_resizable(true);
        col->set_sort_column(i);
        if (i == 0) {
            col->set_min_width(200);
        } else
            col->set_min_width(50);
    }

    // Root
    m_Root = Gtk::VBox();
    m_Root.set_homogeneous(false);

    m_ScrolledWindow.add(m_ServicesTreeView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.property_vexpand() = true;

    m_ScrolledWindow.property_vexpand() = true;
    m_Root.pack_start(m_ScrolledWindow, true, true);

    // Load Data
    LoadData();
}

void ServicesView::LoadData() {
    m_ServicesTreeModel->clear();

    for(const auto& entry : ServicesApi::GetAllServices()) {
        auto row = m_ServicesTreeModel->append();
        const auto& treeRow = *row;

        treeRow[m_ServicesTreeModelColumns.m_Name] = entry.name;
        treeRow[m_ServicesTreeModelColumns.m_Status] = entry.status;
    }

    m_ServicesTreeModel->set_sort_column(0, Gtk::SortType::SORT_ASCENDING);
}

void ServicesView::OnShown() {
}

void ServicesView::OnHidden() {
}

Gtk::Widget &ServicesView::GetRootWidget() {
    return m_Root;
}

void ServicesView::OnRowSelected() {

}

void ServicesView::OnRowClick(GdkEventButton *button) {

}

std::optional<Gtk::TreeIter> ServicesView::GetSelectedIter() {
    auto selection = m_ServicesTreeView.get_selection();
    auto iter = selection->get_selected();
    if(m_ServicesTreeModel->iter_is_valid(iter)) {
        return iter;
    }

    return std::nullopt;
}
