#ifndef WSYSMON_AUTOSTARTVIEW_H
#define WSYSMON_AUTOSTARTVIEW_H

#include <gtkmm.h>
#include <optional>

#include "View.h"

class AutostartTreeModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    AutostartTreeModelColumns() {
        add(m_Name);
        add(m_Status);

        add(m_Path);
    }

    void CreateColumns(Gtk::TreeView& treeView) const {
        treeView.append_column("Name", m_Name);
        treeView.append_column("Status", m_Status);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_Name;
    Gtk::TreeModelColumn<Glib::ustring> m_Status;

    Gtk::TreeModelColumn<Glib::ustring> m_Path;
};

class AutostartView : public View {
public:
    explicit AutostartView(MainWindow *window);
    ~AutostartView() = default;

    void OnShown() override;
    void OnHidden() override;
    Gtk::Widget& GetRootWidget() override;

private:
    void LoadData();

    void OnRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *column);
    void OnRowClick(GdkEventButton *button);

    void OnEnableClick(GdkEventButton *button);

    std::optional<Gtk::TreeIter> GetSelectedIter();

    // Tree view
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::TreeView m_AutostartTreeView;
    Glib::RefPtr<Gtk::TreeStore> m_AutostartTreeModel;
    AutostartTreeModelColumns m_AutostartTreeModelColumns;

    // Controls
    Gtk::VBox m_Root;
    Gtk::Box m_ButtonBox;
    Gtk::Button m_EnableButton;
};


#endif //WSYSMON_AUTOSTARTVIEW_H
