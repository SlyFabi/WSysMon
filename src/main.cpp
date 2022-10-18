#include <gtkmm/application.h>
#include "ui/MainWindow.h"
#include "utils/linux/X11Utils.h"

int main(int argc, char *argv[]) {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    X11Utils::Init();

    auto app = Gtk::Application::create("com.slyfabi.WSysMon");
    MainWindow window(app);

    auto result = app->run(window, argc, argv);
    X11Utils::Dispose();
    return result;
}
