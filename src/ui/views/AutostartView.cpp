#include "AutostartView.h"

#include "../../api/AutostartApi.h"

AutostartView::AutostartView(MainWindow *window) : View(window) {
    // Tree view
    m_AutostartTreeModel = Gtk::TreeStore::create(m_AutostartTreeModelColumns);
    m_AutostartTreeView.set_model(m_AutostartTreeModel);

    m_AutostartTreeView.set_activate_on_single_click(true);
    m_AutostartTreeView.signal_cursor_changed().connect_notify(sigc::mem_fun(*this, &AutostartView::OnRowSelected), false);
    m_AutostartTreeView.signal_button_release_event().connect_notify(sigc::mem_fun(*this, &AutostartView::OnRowClick), false);
    m_AutostartTreeModelColumns.CreateColumns(m_AutostartTreeView);

    // Column size
    for(int i = 0; i < m_AutostartTreeView.get_n_columns(); i++) {
        auto col = m_AutostartTreeView.get_column(i);
        col->set_resizable(true);
        col->set_sort_column(i);
        if (i == 0) {
            col->set_min_width(200);
        } else
            col->set_min_width(50);
    }

    // Render handler
    auto nameColumn = m_AutostartTreeView.get_column(0);
    auto nameRenderer = m_AutostartTreeView.get_column_cell_renderer(0);
    nameColumn->set_cell_data_func(*nameRenderer, [this](Gtk::CellRenderer *renderer, const Gtk::TreeModel::const_iterator &iter) {
        auto rendererText = (Gtk::CellRendererText *)renderer;
        rendererText->set_padding(10, 15);
        rendererText->set_property("weight", 400);
    });

    // Root
    m_Root = Gtk::VBox();
    m_Root.set_homogeneous(false);

    m_ButtonBox = Gtk::HBox();
    m_ButtonBox.property_margin() = 20;
    m_ButtonBox.set_homogeneous(false);

    m_EnableButton = Gtk::Button("Enable");
    m_EnableButton.signal_button_press_event().connect_notify(sigc::mem_fun(*this, &AutostartView::OnEnableClick), false);
    m_EnableButton.set_alignment(1, 0);
    m_EnableButton.set_sensitive(false);
    m_ButtonBox.pack_end(m_EnableButton, Gtk::PackOptions::PACK_SHRINK);

    m_ScrolledWindow.add(m_AutostartTreeView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.property_vexpand() = true;

    m_ScrolledWindow.property_vexpand() = true;
    m_ButtonBox.property_hexpand() = true;
    m_ButtonBox.property_vexpand() = false;
    m_Root.pack_start(m_ScrolledWindow, true, true);
    m_Root.pack_end(m_ButtonBox, false, false);

    // Load Data
    LoadData();
}

void AutostartView::LoadData() {
    m_EnableButton.set_sensitive(false);
    m_AutostartTreeModel->clear();

    for(const auto& entry : AutostartApi::GetEntries()) {
        auto row = m_AutostartTreeModel->append();
        const auto& treeRow = *row;

        auto statusText = "Disabled";
        if(entry.enabled)
            statusText = "Enabled";

        treeRow[m_AutostartTreeModelColumns.m_Path] = entry.path;
        treeRow[m_AutostartTreeModelColumns.m_Name] = entry.name;
        treeRow[m_AutostartTreeModelColumns.m_Status] = statusText;
    }

    m_AutostartTreeModel->set_sort_column(0, Gtk::SortType::SORT_ASCENDING);
}

void AutostartView::OnShown() {
}

void AutostartView::OnHidden() {
}

Gtk::Widget &AutostartView::GetRootWidget() {
    return m_Root;
}

void AutostartView::OnRowSelected() {
    m_EnableButton.set_sensitive(false);
    auto iter = GetSelectedIter();
    if(!iter.has_value())
        return;

    auto row = *iter.value();
    Glib::ustring rowPath = row[m_AutostartTreeModelColumns.m_Path];

    auto entry = AutostartApi::GetEntryAt(rowPath);
    if(entry.has_value()) {
        m_EnableButton.set_sensitive(true);
        if(entry.value().enabled) {
            m_EnableButton.set_label("Disable");
        } else {
            m_EnableButton.set_label("Enable");
        }
    }
}

void AutostartView::OnRowClick(GdkEventButton *button) {

}

void AutostartView::OnEnableClick(GdkEventButton *button) {
    auto iter = GetSelectedIter();
    if(!iter.has_value())
        return;

    auto row = *iter.value();
    Glib::ustring rowPath = row[m_AutostartTreeModelColumns.m_Path];
    auto entry = AutostartApi::GetEntryAt(rowPath);
    if(!entry.has_value())
        return;

    AutostartApi::SetEntryEnabled(entry.value(), !entry.value().enabled);
    LoadData();
}

std::optional<Gtk::TreeIter> AutostartView::GetSelectedIter() {
    auto selection = m_AutostartTreeView.get_selection();
    auto iter = selection->get_selected();
    if(m_AutostartTreeModel->iter_is_valid(iter)) {
        return iter;
    }

    return std::nullopt;
}
