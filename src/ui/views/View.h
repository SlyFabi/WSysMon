#ifndef WSYSMON_VIEW_H
#define WSYSMON_VIEW_H

#include <gtkmm.h>

class MainWindow;

class View {
public:
    explicit View(MainWindow *window) {
        m_Window = window;
    }

    virtual void OnShown() = 0;
    virtual void OnHidden() = 0;

    virtual Gtk::Widget& GetRootWidget() = 0;

protected:
    MainWindow *m_Window;
};

#endif //WSYSMON_VIEW_H
