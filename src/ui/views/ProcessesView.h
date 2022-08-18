#ifndef WSYSMON_PROCESSESVIEW_H
#define WSYSMON_PROCESSESVIEW_H

#include <optional>
#include <vector>

#include <gtkmm.h>

#include "../../api/process/ProcessNode.h"
#include "../../utils/DispatcherThread.h"
#include "View.h"

class ProcessesTreeModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ProcessesTreeModelColumns() {
        add(m_Name);
        add(m_Status);
        add(m_CPUUsage);
        add(m_RAMUsage);
        add(m_DiskUsage);
        //add(m_NetworkUsage);
        add(m_GPUMemoryUsage);

        add(m_CPUColorAlpha);
        add(m_RAMColorAlpha);
        add(m_DiskColorAlpha);

        add(m_Stub);
        add(m_Category);
        add(m_Pid);
    }

    void CreateColumns(Gtk::TreeView& treeView) const {
        treeView.append_column("Name", m_Name);
        treeView.append_column("Status", m_Status);
        treeView.append_column("CPU", m_CPUUsage);
        treeView.append_column("RAM", m_RAMUsage);
        treeView.append_column("Disk", m_DiskUsage);
        //treeView.append_column("Network", m_NetworkUsage);
        treeView.append_column("GPU Memory", m_GPUMemoryUsage);

        treeView.append_column("PID", m_Pid);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_Name;
    Gtk::TreeModelColumn<Glib::ustring> m_Status;
    Gtk::TreeModelColumn<Glib::ustring> m_CPUUsage;
    Gtk::TreeModelColumn<Glib::ustring> m_RAMUsage;
    Gtk::TreeModelColumn<Glib::ustring> m_DiskUsage;
    //Gtk::TreeModelColumn<Glib::ustring> m_NetworkUsage;
    Gtk::TreeModelColumn<Glib::ustring> m_GPUMemoryUsage;

    Gtk::TreeModelColumn<double> m_CPUColorAlpha;
    Gtk::TreeModelColumn<double> m_RAMColorAlpha;
    Gtk::TreeModelColumn<double> m_DiskColorAlpha;

    Gtk::TreeModelColumn<Glib::ustring> m_Stub;
    Gtk::TreeModelColumn<int> m_Category;
    Gtk::TreeModelColumn<int> m_Pid;
};

class ProcessesView : public View {
public:
    explicit ProcessesView(MainWindow *window);
    ~ProcessesView();

    void OnShown() override;
    void OnHidden() override;

    void OnActionEndTask();
    void OnActionKillTask();
    void OnActionProcessProperties();

    Gtk::Widget& GetRootWidget() override;

    void UpdateCategory(int categoryId, ProcessNode *procRoot);

private:
    void OnRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *column);
    void OnRowClick(GdkEventButton *button);
    void OnKillClick(GdkEventButton *button);

    std::optional<Gtk::TreeIter> GetSelectedIter();

    void UpdateCategoryProcess(int categoryId, ProcessNode *procNode);
    Gtk::TreeIter AddProcess(int categoryId, ProcessNode *procNode, std::optional<Gtk::TreeIter> parent);
    Gtk::TreeIter GetSortedInsertionRow(int categoryId, ProcessNode *procNode);

    std::vector<Gtk::TreeIter> GetAllByCategory(int categoryId, bool cache = true);
    std::vector<Gtk::TreeIter> GetAllByCategoryIter(int categoryId, const Gtk::TreeNodeChildren& children);

    std::optional<Gtk::TreeIter> GetRowByPid(int categoryId, int pid);
    bool IsCategory(const Gtk::TreeIter& row);

    Gtk::ScrolledWindow m_ScrolledWindow;

    Gtk::TreeView m_ProcessTreeView;
    Glib::RefPtr<Gtk::TreeStore> m_ProcessTreeModel;
    ProcessesTreeModelColumns m_ProcessTreeModelColumns;

    // Controls
    Gtk::VBox m_Root;
    Gtk::Box m_ButtonBox;
    Gtk::Button m_KillButton;
    Gtk::Menu m_RightClickMenu;

    // Categories
    Gtk::TreeIter m_CategoryApps;
    Gtk::TreeIter m_CategoryWine;
    Gtk::TreeIter m_CategoryBackground;
    Gtk::TreeIter m_CategorySystem;

    // Update thread
    DispatcherThread *m_UpdateThread;

    // Cache
    std::map<int, std::vector<Gtk::TreeIter>> m_CategoryCache;

    ProcessNode *m_AppProcesses{};
    ProcessNode *m_WineProcesses{};
    ProcessNode *m_BackgroundProcesses{};
    ProcessNode *m_SystemProcesses{};
};


#endif //WSYSMON_PROCESSESVIEW_H
