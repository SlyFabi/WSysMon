#ifndef WSYSMON_X11UTILS_H
#define WSYSMON_X11UTILS_H

#include <iostream>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class X11Utils {
public:
    static void Init();
    static void Dispose();

    static std::vector<Window> GetAllWindows();
    static std::vector<int> GetAllPidsWithWindows();

private:
    static std::vector<Window> GetAllWindowsIter(Display *display, Window window);

    static unsigned char *GetWindowProperty(Display *display, Window window, const char *propertyName);

    static _XDisplay *m_Display;

    X11Utils() = default;
};

#endif //WSYSMON_X11UTILS_H
