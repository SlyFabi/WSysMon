#include "X11Utils.h"

_XDisplay *X11Utils::m_Display = nullptr;

void X11Utils::Init() {
    m_Display = XOpenDisplay(nullptr);
}

void X11Utils::Dispose() {
    if(m_Display != nullptr) {
        XCloseDisplay(m_Display);
        m_Display = nullptr;
    }
}

std::vector<int> X11Utils::GetAllPidsWithWindows() {
    std::vector<int> result;
    if(m_Display == nullptr)
        return result;

    auto windowList = GetAllWindows();
    for(auto window : windowList) {
        XWindowAttributes wattr{};
        XGetWindowAttributes(m_Display, window, &wattr);
        if (wattr.map_state != IsViewable)
            continue;

        auto data = GetWindowProperty(m_Display, window, "_NET_WM_PID");
        if (data != nullptr) {
            int window_pid = (int)*((unsigned long *)data);
            result.push_back(window_pid);
        }
        free(data);
    }

    return result;
}

std::vector<Window> X11Utils::GetAllWindows() {
    if(m_Display == nullptr)
        return {};

    auto root = XDefaultRootWindow(m_Display);
    return GetAllWindowsIter(m_Display, root);
}

std::vector<Window> X11Utils::GetAllWindowsIter(Display *display, Window window) {
    std::vector<Window> result;
    result.push_back(window);

    Window root{}, parent{};
    Window* children = nullptr;
    unsigned int n = 0;
    XQueryTree(display, window, &root, &parent, &children, &n);
    if (children != nullptr) {
        for(int i = 0; i < n; i++) {
            auto childList = GetAllWindowsIter(display, children[i]);
            result.insert(std::end(result), std::begin(childList), std::end(childList));
        }
        XFree(children);
    }

    return result;
}

unsigned char *X11Utils::GetWindowProperty(Display *display, Window window, const char *propertyName) {
    Atom type{};
    int format = 0;
    unsigned long count = 0;
    unsigned long tmpL = 0;
    unsigned char *property = nullptr;

    auto atom = XInternAtom(m_Display, propertyName, False);
    auto status = XGetWindowProperty(display, window, atom, 0, (~0L), False, AnyPropertyType, &type, &format, &count, &tmpL, &property);
    if(status != Success || count <= 0)
        return nullptr;

    return property;
}
