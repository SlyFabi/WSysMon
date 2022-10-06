#include "MainWindow.h"
#include "../storage/AppSettings.h"

#define MAIN_WINDOW_PAGE_PROCESSES "Processes"
#define MAIN_WINDOW_PAGE_PERFORMANCE "Performance"
#define MAIN_WINDOW_PAGE_AUTOSTART "Autostart"

MainWindow::MainWindow(const Glib::RefPtr<Gtk::Application>& application) {
    m_Application = application;

    set_title("WSysMon");
    set_default_size(0, 500);
    set_position(Gtk::WIN_POS_CENTER);

    // Save window size on exit
    signal_delete_event().connect([this](GdkEventAny* event) {
        int width, height;
        get_size(width, height);

        auto settings = AppSettings::Get();
        settings.windowWidth = width;
        settings.windowHeight = height;
        AppSettings::Save(settings);
        return false;
    });

    // Window size
    auto settings = AppSettings::Get();
    if(settings.windowWidth != 0 && settings.windowHeight != 0) {
        set_default_size(settings.windowWidth, settings.windowHeight);
    } else {
        auto display = Gdk::Display::get_default();
        if(display) {
            auto monitor = display->get_primary_monitor();
            if(monitor) {
                Gdk::Rectangle screenSize;
                monitor->get_workarea(screenSize);
                set_default_size(0, (int)((double)screenSize.get_height() * 0.6));
            }
        }
    }

    // Processes View
    m_ProcessView = new ProcessesView(this);
    m_MainStack.add(m_ProcessView->GetRootWidget(), MAIN_WINDOW_PAGE_PROCESSES, MAIN_WINDOW_PAGE_PROCESSES);

    // Performance View
    m_PerformanceView = new PerformanceView(this);
    m_MainStack.add(m_PerformanceView->GetRootWidget(), MAIN_WINDOW_PAGE_PERFORMANCE, MAIN_WINDOW_PAGE_PERFORMANCE);

    // Autostart View
    m_AutostartView = new AutostartView(this);
    m_MainStack.add(m_AutostartView->GetRootWidget(), MAIN_WINDOW_PAGE_AUTOSTART, MAIN_WINDOW_PAGE_AUTOSTART);

    // Main Tabber
    m_MainTabber.set_stack(m_MainStack);
    m_MainStack.property_visible_child().signal_changed().connect([this] {
        OnTabChanged();
    });
    add(m_MainStack);

    // Header Bar
    m_HeaderBar.set_show_close_button(true);
    m_HeaderBar.set_custom_title(m_MainTabber);
    set_titlebar(m_HeaderBar);

    // Menu Actions
    m_Application->add_action("mainView.about", [] {
        spdlog::debug("About");
    });

    auto procListAction = Gio::SimpleAction::create_bool("mainView.displayProcList",
                                                 AppSettings::Get().displayProcList);
    procListAction->signal_activate().connect([this, procListAction](const Glib::VariantBase& variant) {
        auto settings = AppSettings::Get();
        settings.displayProcList = !settings.displayProcList;
        procListAction->set_state(Glib::Variant<bool>::create(settings.displayProcList));
        AppSettings::Save(settings);
        m_ProcessView->MarkDirty();
    });
    m_Application->add_action(procListAction);

    auto iecAction = Gio::SimpleAction::create_bool("mainView.useIECUnits",
                                                 AppSettings::Get().useIECUnits);
    iecAction->signal_activate().connect([this, iecAction](const Glib::VariantBase& variant) {
        auto settings = AppSettings::Get();
        settings.useIECUnits = !settings.useIECUnits;
        iecAction->set_state(Glib::Variant<bool>::create(settings.useIECUnits));
        AppSettings::Save(settings);
        m_ProcessView->MarkDirty();
    });
    m_Application->add_action(iecAction);

    // Menu
    Glib::ustring ui_info = R""""(
<interface>
  <menu id="menu-mainOptionsMenu">
    <!--<section>
      <item>
        <attribute name="label" translatable="yes">About</attribute>
        <attribute name="action">app.mainView.about</attribute>
      </item>
    </section>-->
    <section>
      <item>
        <attribute name="label">Display as list</attribute>
        <attribute name="action">app.mainView.displayProcList</attribute>
      </item>
      <item>
        <attribute name="label">Use IEC units</attribute>
        <attribute name="action">app.mainView.useIECUnits</attribute>
      </item>
    </section>
  </menu>
</interface>
)"""";
    auto builder = Gtk::Builder::create();
    builder->add_from_string(ui_info);

    auto object = builder->get_object("menu-mainOptionsMenu");
    auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
    m_OptionsMenu = Gtk::Menu(gmenu);

    m_MenuButton.set_menu(m_OptionsMenu);
    m_MenuButton.set_image_from_icon_name("open-menu-symbolic");
    m_HeaderBar.pack_end(m_MenuButton);

    this->signal_show().connect(sigc::mem_fun(*this, &MainWindow::OnShown));
    show_all_children();
}

MainWindow::~MainWindow() {
    SAFE_DELETE(m_ProcessView);
    SAFE_DELETE(m_PerformanceView);
    SAFE_DELETE(m_AutostartView);
}

void MainWindow::OnShown() {
    m_ProcessView->OnShown();
}

void MainWindow::OnTabChanged() {
    if(m_ProcessView == nullptr || m_PerformanceView == nullptr || m_AutostartView == nullptr)
        return;

    auto currPage = m_MainStack.get_visible_child_name();
    if(currPage == MAIN_WINDOW_PAGE_PROCESSES) {
        m_ProcessView->OnShown();
        m_PerformanceView->OnHidden();
        m_AutostartView->OnHidden();
    } else if(currPage == MAIN_WINDOW_PAGE_PERFORMANCE) {
        m_PerformanceView->OnShown();
        m_ProcessView->OnHidden();
        m_AutostartView->OnHidden();
    } else if(currPage == MAIN_WINDOW_PAGE_AUTOSTART) {
        m_AutostartView->OnShown();
        m_ProcessView->OnHidden();
        m_PerformanceView->OnHidden();
    }
}

Gtk::Application *MainWindow::GetApp() {
    return m_Application.get();
}
