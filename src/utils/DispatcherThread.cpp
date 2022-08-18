#include "DispatcherThread.h"

DispatcherThread::DispatcherThread(const std::function<void(void)>& loopFunc, const std::function<void(void)>& onDispatchFunc) {
    m_LoopFunction = loopFunc;
    m_OnDispatchFunction = onDispatchFunc;

    m_Dispatcher.connect(sigc::mem_fun(*this, &DispatcherThread::OnThreadDispatch));
    m_Running = false;
}

void DispatcherThread::Start() {
    if(m_Running)
        return;

    m_Running = true;
    m_Thread = std::thread([this] { ThreadLoop(); });
}

void DispatcherThread::Stop(bool blocking) {
    if(!m_Running)
        return;

    m_Running = false;
    if(!blocking) {
        m_Thread.detach();
    } else {
        m_Thread.join();
    }
}

void DispatcherThread::Dispatch() {
    m_Mutex.lock();
    m_Dispatcher.emit();
}

void DispatcherThread::ThreadLoop() {
    while(m_Running) {
        m_Mutex.lock();
        m_Mutex.unlock();
        m_LoopFunction();
    }
}

void DispatcherThread::OnThreadDispatch() {
    m_OnDispatchFunction();
    m_Mutex.unlock();
}
