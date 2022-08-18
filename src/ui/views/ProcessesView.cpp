#include "ProcessesView.h"
#include "../../api/process/ProcessManager.h"
#include "../../utils/UnitConverter.h"
#include "../MainWindow.h"

#include <chrono>
#include <spdlog/fmt/fmt.h>
#include <gtkmm/popovermenu.h>

ProcessesView::ProcessesView(MainWindow *window)
    : View(window) {
    // Tree view
    m_ProcessTreeModel = Gtk::TreeStore::create(m_ProcessTreeModelColumns);
    m_ProcessTreeView.set_model(m_ProcessTreeModel);

    m_ProcessTreeView.set_activate_on_single_click(true);
    m_ProcessTreeView.signal_row_activated().connect_notify(sigc::mem_fun(*this, &ProcessesView::OnRowActivated), false);
    m_ProcessTreeView.signal_button_release_event().connect_notify(sigc::mem_fun(*this, &ProcessesView::OnRowClick), false);

    // Menu
    m_Window->GetApp()->add_action("processesView.endTask", sigc::mem_fun(*this, &ProcessesView::OnActionEndTask));
    m_Window->GetApp()->set_accel_for_action("processesView.endTask", "<Primary>t");

    m_Window->GetApp()->add_action("processesView.killTask", sigc::mem_fun(*this, &ProcessesView::OnActionKillTask));
    m_Window->GetApp()->set_accel_for_action("processesView.killTask", "<Primary>k");

    Glib::ustring ui_info =
            "<interface>"
            "  <menu id='menu-processesViewRightClick'>"
            "    <section>"
            "      <item>"
            "        <attribute name='label' translatable='yes'>End Task</attribute>"
            "        <attribute name='action'>app.processesView.endTask</attribute>"
            "      </item>"
            "      <item>"
            "        <attribute name='label' translatable='yes'>Kill Task</attribute>"
            "        <attribute name='action'>app.processesView.killTask</attribute>"
            "      </item>"
            "      <item>"
            "        <attribute name='label' translatable='yes'>Properties</attribute>"
            "        <attribute name='action'>app.processesView.Properties</attribute>"
            "      </item>"
            "    </section>"
            "  </menu>"
            "</interface>";

    auto builder = Gtk::Builder::create();
    builder->add_from_string(ui_info);

    auto object = builder->get_object("menu-processesViewRightClick");
    auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
    m_RightClickMenu = Gtk::Menu(gmenu);

    // Controls
    m_Root = Gtk::VBox();
    m_Root.set_homogeneous(false);

    m_ButtonBox = Gtk::HBox();
    m_ButtonBox.property_margin() = 20;
    m_ButtonBox.set_homogeneous(false);

    m_KillButton = Gtk::Button("End task");
    m_KillButton.signal_button_press_event().connect_notify(sigc::mem_fun(*this, &ProcessesView::OnKillClick), false);
    m_KillButton.set_alignment(1, 0);
    m_KillButton.set_sensitive(false);

    m_ButtonBox.pack_end(m_KillButton, Gtk::PackOptions::PACK_SHRINK);

    // Categories
    m_CategoryApps = m_ProcessTreeModel->append();
    (*m_CategoryApps)[m_ProcessTreeModelColumns.m_Name] = "Apps";
    m_CategoryWine = m_ProcessTreeModel->append();
    (*m_CategoryWine)[m_ProcessTreeModelColumns.m_Name] = "Wine";
    m_CategoryBackground = m_ProcessTreeModel->append();
    (*m_CategoryBackground)[m_ProcessTreeModelColumns.m_Name] = "Background";
    m_CategorySystem = m_ProcessTreeModel->append();
    (*m_CategorySystem)[m_ProcessTreeModelColumns.m_Name] = "System";

    m_ProcessTreeModelColumns.CreateColumns(m_ProcessTreeView);

    // Render handler
    auto nameColumn = m_ProcessTreeView.get_column(0);
    auto nameRenderer = m_ProcessTreeView.get_column_cell_renderer(0);
    nameColumn->set_cell_data_func(*nameRenderer, [this](Gtk::CellRenderer *renderer, const Gtk::TreeModel::const_iterator &iter) {
        auto rendererText = (Gtk::CellRendererText *)renderer;
        if(IsCategory(*iter)) {
            //rendererText->property_size().set_value(18);
            rendererText->set_padding(0, 10);
            rendererText->set_property("weight", 800);
        } else {
            rendererText->set_padding(10, 15);
            rendererText->set_property("weight", 400);
        }
    });

    auto pidColumn = m_ProcessTreeView.get_column(6);
    auto pidRenderer = m_ProcessTreeView.get_column_cell_renderer(6);
    pidColumn->set_cell_data_func(*pidRenderer, [this](Gtk::CellRenderer *renderer, const Gtk::TreeModel::const_iterator &iter) {
        auto rendererText = (Gtk::CellRendererText *)renderer;
        rendererText->set_visible(!IsCategory(*iter));
    });

    for(int i = 2; i <= 5; i++) {
        auto column = m_ProcessTreeView.get_column(i);
        auto renderer = m_ProcessTreeView.get_column_cell_renderer(i);
        column->set_cell_data_func(*renderer, [this, i](Gtk::CellRenderer *renderer, const Gtk::TreeModel::const_iterator &iter) {
            if(!IsCategory(*iter)) {
                auto textRenderer = (Gtk::CellRendererText *)renderer;
                const auto& row = *iter;
                auto alpha = 0.;
                if(i == 2) { // CPU
                    alpha = (double)row.get_value(m_ProcessTreeModelColumns.m_CPUColorAlpha);
                } else if(i == 3) { // RAM
                    alpha = (double)row.get_value(m_ProcessTreeModelColumns.m_RAMColorAlpha);
                } else if(i == 4) { // Disk
                    alpha = (double)row.get_value(m_ProcessTreeModelColumns.m_DiskColorAlpha);
                }

                if(alpha > 0.6) {
                    auto rgba = Gdk::RGBA();
                    rgba.set_rgba(0.0, 0.0, 0.0, 1.0);
                    textRenderer->property_foreground_rgba().set_value(rgba);
                } else {
                    Gdk::RGBA textColor;
                    m_ProcessTreeView.get_style_context()->lookup_color("theme_text_color", textColor);
                    textRenderer->property_foreground_rgba().set_value(textColor);
                }

                auto rgba = Gdk::RGBA();
                rgba.set_rgba(1.0, 0.8203125, 0.390625, alpha);
                renderer->property_cell_background_rgba().set_value(rgba);
            } else {
                auto rgba = Gdk::RGBA();
                rgba.set_rgba(0.0, 0.0, 0.0, 0.0);
                renderer->property_cell_background_rgba().set_value(rgba);
            }
        });
    }

    // Scroll bar
    m_ScrolledWindow.add(m_ProcessTreeView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_ScrolledWindow.property_vexpand() = true;
    m_ButtonBox.property_hexpand() = true;
    m_ButtonBox.property_vexpand() = false;
    m_Root.pack_start(m_ScrolledWindow, true, true);
    m_Root.pack_end(m_ButtonBox, false, false);

    // Update thread
    m_UpdateThread = new DispatcherThread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            auto start = Utils::GetCurrentTimeMS();

            ProcessManager::ClearCache();
            m_AppProcesses = ProcessManager::GetProcessTreeByCategory(PROCESSES_VIEW_CATEGORY_APPS);
            m_WineProcesses = ProcessManager::GetProcessTreeByCategory(PROCESSES_VIEW_CATEGORY_WINE);
            m_BackgroundProcesses = ProcessManager::GetProcessTreeByCategory(PROCESSES_VIEW_CATEGORY_BACKGROUND);
            m_SystemProcesses = ProcessManager::GetProcessTreeByCategory(PROCESSES_VIEW_CATEGORY_SYSTEM);

            auto took = Utils::GetCurrentTimeMS() - start;
            spdlog::debug("Tree took {}ms", took);

            m_UpdateThread->Dispatch();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }, [this]() {
            m_CategoryCache.clear();
            UpdateCategory(PROCESSES_VIEW_CATEGORY_APPS, m_AppProcesses);
            UpdateCategory(PROCESSES_VIEW_CATEGORY_WINE, m_WineProcesses);
            UpdateCategory(PROCESSES_VIEW_CATEGORY_BACKGROUND, m_BackgroundProcesses);
            UpdateCategory(PROCESSES_VIEW_CATEGORY_SYSTEM, m_SystemProcesses);
        });
}

ProcessesView::~ProcessesView() {
    m_UpdateThread->Stop();
    SAFE_DELETE(m_UpdateThread);

    ProcessManager::ClearCache();
}

void ProcessesView::OnShown() {
    if(m_UpdateThread != nullptr)
        m_UpdateThread->Start();
}

void ProcessesView::OnHidden() {
    if(m_UpdateThread != nullptr)
        m_UpdateThread->Stop(false);
}

void ProcessesView::OnActionEndTask() {
    auto iter = GetSelectedIter();
    if(iter != std::nullopt) {
        auto row = *iter.value();
        auto pid = (int)row.get_value(m_ProcessTreeModelColumns.m_Pid);
        ProcessManager::KillByPid(pid);
    }
}

void ProcessesView::OnActionKillTask() {
    auto iter = GetSelectedIter();
    if(iter != std::nullopt) {
        auto row = *iter.value();
        auto pid = (int)row.get_value(m_ProcessTreeModelColumns.m_Pid);
        ProcessManager::KillByPid(pid, true);
    }
}

void ProcessesView::OnActionProcessProperties() {

}

void ProcessesView::OnRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *column) {
    auto iter = GetSelectedIter();
    m_KillButton.set_sensitive(iter != std::nullopt && !IsCategory(iter.value()));
}

void ProcessesView::OnRowClick(GdkEventButton *button) {
    if (button->type == GdkEventType::GDK_BUTTON_RELEASE && button->button == 3) {
        auto iter = GetSelectedIter();
        if(iter != std::nullopt) {
            auto row = *iter.value();
            spdlog::debug("Row: {}", row.get_value(m_ProcessTreeModelColumns.m_Name).c_str());

            // Menu
            if(!IsCategory(iter.value())) {
                if(!m_RightClickMenu.get_attach_widget())
                    m_RightClickMenu.attach_to_widget(*m_Window);
                m_RightClickMenu.popup(button->button, button->time);
            }
        }
    }
}

void ProcessesView::OnKillClick(GdkEventButton *button) {
    if(button->type != GdkEventType::GDK_BUTTON_PRESS || button->button != 1)
        return;

    auto iter = GetSelectedIter();
    if(iter != std::nullopt) {
        auto row = *iter.value();
        spdlog::debug("Row: {}", row.get_value(m_ProcessTreeModelColumns.m_Name).c_str());

        auto pid = (int)row.get_value(m_ProcessTreeModelColumns.m_Pid);
        ProcessManager::KillByPid(pid);
    }
}

Gtk::Widget& ProcessesView::GetRootWidget() {
    return m_Root;
}

std::optional<Gtk::TreeIter> ProcessesView::GetSelectedIter() {
    auto selection = m_ProcessTreeView.get_selection();
    auto iter = selection->get_selected();
    if(m_ProcessTreeModel->iter_is_valid(iter)) {
        return iter;
    }

    return std::nullopt;
}

void ProcessesView::UpdateCategory(int categoryId, ProcessNode *procRoot) {
    if(procRoot == nullptr)
        return;

    // Remove dead
    auto procList = procRoot->FlatTree();
    for(const auto& row : GetAllByCategory(categoryId)) {
        auto rowPid = (int)(*row)[m_ProcessTreeModelColumns.m_Pid];
        auto found = std::find_if(procList.begin(), procList.end(),[&](ProcessNode *n) -> bool { return n->GetPid() == rowPid; });
        if(found == std::end(procList)) {
            m_ProcessTreeModel->erase(row);
        }
    }

    // Clear cache to refresh
    m_CategoryCache.clear();

    // Update processes
    for(auto procNode : procRoot->GetChildren())
        UpdateCategoryProcess(categoryId, procNode);
}

void ProcessesView::UpdateCategoryProcess(int categoryId, ProcessNode *procNode) {
    auto row = GetRowByPid(categoryId, procNode->GetPid());
    if(row == std::nullopt) {
        if(procNode->GetParent() != nullptr) {
            auto parentRow = GetRowByPid(categoryId, procNode->GetParent()->GetPid());
            if(parentRow != std::nullopt)
                row = AddProcess(categoryId, procNode, parentRow);
            else
                row = AddProcess(categoryId, procNode, std::nullopt);
        } else {
            row = AddProcess(categoryId, procNode, std::nullopt);
        }
    }

    // Required
    auto treeRow = *(row.value());
    treeRow[m_ProcessTreeModelColumns.m_Pid] = procNode->GetPid();
    treeRow[m_ProcessTreeModelColumns.m_Category] = categoryId;

    // Info
    treeRow[m_ProcessTreeModelColumns.m_Name] = procNode->GetName();
    treeRow[m_ProcessTreeModelColumns.m_Status] = procNode->GetStatus();
    treeRow[m_ProcessTreeModelColumns.m_CPUUsage] = fmt::format("{:.2f}%", procNode->GetCPUUsage());
    treeRow[m_ProcessTreeModelColumns.m_RAMUsage] = UnitConverter::ConvertBytesString(procNode->GetRAMUsage(), UnitType::AUTO);
    treeRow[m_ProcessTreeModelColumns.m_DiskUsage] = UnitConverter::ConvertBytesString(procNode->GetDiskUsage(), UnitType::AUTO);
    //treeRow[m_ProcessTreeModelColumns.m_NetworkUsage] = UnitConverter::ConvertBytesString(procNode->GetNetworkUsage(), UnitType::AUTO);

    if(procNode->GetGPUInfo().memoryUsage > 0)
        treeRow[m_ProcessTreeModelColumns.m_GPUMemoryUsage] = UnitConverter::ConvertBytesString(procNode->GetGPUInfo().memoryUsage, UnitType::AUTO);

    // Colors
    auto diskGB = (double)procNode->GetDiskUsage() / 1000. / 1000. / 1000.;
    auto ramGB = (double)procNode->GetRAMUsage() / 1000. / 1000. / 1000.;

    auto cpuAlpha = 0.1 + procNode->GetCPUUsage() / 40.;
    auto ramAlpha = 0.1 + ramGB * 1.2;
    auto diskAlpha =  0.1 + diskGB * 2;

    treeRow[m_ProcessTreeModelColumns.m_CPUColorAlpha] = cpuAlpha;
    treeRow[m_ProcessTreeModelColumns.m_RAMColorAlpha] = ramAlpha;
    treeRow[m_ProcessTreeModelColumns.m_DiskColorAlpha] = diskAlpha;

    for(auto child : procNode->GetChildren())
        UpdateCategoryProcess(categoryId, child);
}

Gtk::TreeIter ProcessesView::AddProcess(int categoryId, ProcessNode *procNode, std::optional<Gtk::TreeIter> parent) {
    Gtk::TreeIter rowToInsert;
    Gtk::TreeIter newRow;
    if(parent != std::nullopt) {
        rowToInsert = parent.value();
        auto realRow = (*rowToInsert);
        newRow = m_ProcessTreeModel->append(realRow.children());
        m_CategoryCache[categoryId].push_back(*newRow);
    } else {
        auto row = GetSortedInsertionRow(categoryId, procNode);
        newRow = m_ProcessTreeModel->insert_after(row);

        auto cacheIter = std::find(m_CategoryCache[categoryId].begin(), m_CategoryCache[categoryId].end(), row);
        if(cacheIter != m_CategoryCache[categoryId].end()) {
            m_CategoryCache[categoryId].insert(cacheIter, *newRow);
        } else {
            m_CategoryCache[categoryId].push_back(*newRow);
        }
    }

    return *newRow;
}

Gtk::TreeIter ProcessesView::GetSortedInsertionRow(int categoryId, ProcessNode *procNode) {
    auto categoryRows = GetAllByCategory(categoryId, false);
    auto insertIter = std::optional<Gtk::TreeIter>();

    auto procNodeName = Utils::stringToLower(procNode->GetName());
    for(const auto& iter : categoryRows) {
        const auto& row = *iter;
        if(row.parent())
            continue;

        auto pid = (int)row.get_value(m_ProcessTreeModelColumns.m_Pid);
        auto proc = ProcessManager::GetProcessByPid(pid);

        auto procName = Utils::stringToLower(proc->GetName());
        if(procNodeName.compare(procName) >= 0) {
            insertIter = iter;
        }
    }

    if(insertIter.has_value())
        return insertIter.value();

    Gtk::TreeIter rowToInsert;
    switch(categoryId) {
        case PROCESSES_VIEW_CATEGORY_APPS:
            rowToInsert = m_CategoryApps;
            break;
        case PROCESSES_VIEW_CATEGORY_WINE:
            rowToInsert = m_CategoryWine;
            break;
        case PROCESSES_VIEW_CATEGORY_BACKGROUND:
            rowToInsert = m_CategoryBackground;
            break;
        case PROCESSES_VIEW_CATEGORY_SYSTEM:
            rowToInsert = m_CategorySystem;
            break;
        default:
            spdlog::error("GetSortedInsertionRow Unknown Category");
            break;
    }

    return rowToInsert;
}

std::vector<Gtk::TreeIter> ProcessesView::GetAllByCategory(int categoryId, bool cache) {
    if(cache) {
        if(Utils::mapContains(m_CategoryCache, categoryId))
            return m_CategoryCache[categoryId];
    }

    auto categoryProcs = GetAllByCategoryIter(categoryId, m_ProcessTreeModel->children());
    m_CategoryCache[categoryId] = categoryProcs;
    return categoryProcs;
}

std::vector<Gtk::TreeIter> ProcessesView::GetAllByCategoryIter(int categoryId, const Gtk::TreeNodeChildren& children) {
    std::vector<Gtk::TreeIter> result;
    for (const auto& row : children) {
        auto catId = (int)row.get_value(m_ProcessTreeModelColumns.m_Category); // This is not set for categories itself
        if(categoryId == catId) {
            result.push_back(row);
        }

        auto childrenResult = GetAllByCategoryIter(categoryId, row.children());
        result.insert(std::end(result), std::begin(childrenResult), std::end(childrenResult));
    }

    return result;
}

std::optional<Gtk::TreeIter> ProcessesView::GetRowByPid(int categoryId, int pid) {
    auto procList = GetAllByCategory(categoryId);
    for(auto row : procList) {
        auto rowPid = (int)(*row).get_value(m_ProcessTreeModelColumns.m_Pid);
        if(rowPid == pid)
            return row;
    }

    return std::nullopt;
}

bool ProcessesView::IsCategory(const Gtk::TreeIter& row) {
    return row == m_CategoryApps || row == m_CategoryWine || row == m_CategoryBackground || row == m_CategorySystem;
}
