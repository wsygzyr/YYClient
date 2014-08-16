#include "MMTimer.h"
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
void WINAPI CALLBACK mmtimer_proc(uint timerId, uint, DWORD_PTR user, DWORD_PTR, DWORD_PTR)
{
    MMTimer *t = (MMTimer*)user;
    emit t->timeout();
}
 MMTimer::MMTimer( QObject *parent) :
    QObject(parent),m_id(0)
{
}
 MMTimer::~MMTimer()
{
    stop();
}
 void MMTimer::start()
{
    m_id = timeSetEvent(m_interval, 1, mmtimer_proc, (DWORD_PTR)this,
                 TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
 }
 void MMTimer::stop()
{
    if (m_id){
        timeKillEvent(m_id);
        m_id = 0;
    }
 }

 void MMTimer::setInterval(int msec)
 {
     m_interval = msec;
 }
