#ifndef WSYSMON_SERVICESVIEW_H
#define WSYSMON_SERVICESVIEW_H

#include <gtkmm.h>
#include <optional>

#include "View.h"

class ServicesTreeModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ServicesTreeModelColumns() {
        add(m_Name);
        add(m_Status);
    }

    void CreateColumns(Gtk::TreeView& treeView) const {
        treeView.append_column("Name", m_Name);
        treeView.append_column("Status", m_Status);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_Name;
    Gtk::TreeModelColumn<Glib::ustring> m_Status;
};

class ServicesView : public View {
public:
    explicit ServicesView(MainWindow *window);
    ~ServicesView() = default;

    void OnShown() override;
    void OnHidden() override;
    Gtk::Widget& GetRootWidget() override;

private:
    void LoadData();

    void OnRowSelected();
    void OnRowClick(GdkEventButton *button);

    std::optional<Gtk::TreeIter> GetSelectedIter();

    // Tree view
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::TreeView m_ServicesTreeView;
    Glib::RefPtr<Gtk::TreeStore> m_ServicesTreeModel;
    ServicesTreeModelColumns m_ServicesTreeModelColumns;

    // Update thread
    //DispatcherThread *m_UpdateThread;

    // Controls
    Gtk::VBox m_Root;
};


#endif //WSYSMON_SERVICESVIEW_H
