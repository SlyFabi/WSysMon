#ifndef WSYSMON_DISPATCHERTHREAD_H
#define WSYSMON_DISPATCHERTHREAD_H

#include <functional>
#include <mutex>
#include <thread>
#include <glibmm/dispatcher.h>

class DispatcherThread {
public:
    DispatcherThread(const std::function<void(void)>& loopFunc, const std::function<void(void)>& onDispatchFunc);

    void Start();
    void Stop(bool blocking = true);

    void Dispatch();

private:
    void OnThreadDispatch();
    void ThreadLoop();

    Glib::Dispatcher m_Dispatcher;
    std::thread m_Thread;
    std::mutex m_Mutex;

    bool m_Running{};

    std::function<void(void)> m_LoopFunction;
    std::function<void(void)> m_OnDispatchFunction;
};


#endif //WSYSMON_DISPATCHERTHREAD_H
