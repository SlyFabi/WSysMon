#ifndef WSYSMON_MAINWINDOW_H
#define WSYSMON_MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>

#include "views/ProcessesView.h"
#include "views/PerformanceView.h"

class MainWindow : public Gtk::Window {
public:
    explicit MainWindow(const Glib::RefPtr<Gtk::Application>& application);
    ~MainWindow() override;

    Gtk::Application *GetApp();

private:
    void OnShown();
    void OnTabChanged();

    Glib::RefPtr<Gtk::Application> m_Application;

    Gtk::HeaderBar m_HeaderBar;
    Gtk::MenuButton m_MenuButton;
    Gtk::Menu m_OptionsMenu;

    Gtk::StackSwitcher m_MainTabber;
    Gtk::Stack m_MainStack;

    ProcessesView *m_ProcessView;
    PerformanceView *m_PerformanceView;
};


#endif //WSYSMON_MAINWINDOW_H
