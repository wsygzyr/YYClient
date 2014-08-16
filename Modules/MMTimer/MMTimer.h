#ifndef MMTIMER_H
#define MMTIMER_H

#include <QObject>
#include <Windows.h>
class MMTimer : public QObject
{
    Q_OBJECT
public:
    explicit MMTimer(QObject *parent = 0);
    ~MMTimer();
 signals:
    void timeout();
 public slots:
    void start();
    void stop();
    void setInterval(int msec);

friend void WINAPI CALLBACK mmtimer_proc(uint, uint, DWORD_PTR, DWORD_PTR, DWORD_PTR);

private:
    int m_interval;
    int m_id;
};

#endif // MMTIMER_H
