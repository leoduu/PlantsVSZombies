#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <QVector>

class Timer;


class TimerManagement : public QObject
{
    Q_OBJECT

public:
    explicit TimerManagement(QObject *parent);

    uint32_t timeCnt() {return cnt;}
    void Start(QObject *parent);
    void Add(Timer *);
    void Detach(Timer *);

signals:
    void update(uint32_t);

private:
    QObject *parent;

    QList<Timer *> timerList;
    QTimer timer;
    uint32_t cnt;
};


class Timer : public QObject
{
    Q_OBJECT

public:
    Timer(TimerManagement *timerMgmt);
    ~Timer();
    bool isTimeout(uint32_t cnt) {return timeoutCnt<=cnt;}
    bool isActive() {return state;}
    void setInterval(int);
    void Start();
    void Start(int);
    void Stop();
    void NextPeriod();

    uint32_t timeoutCnt;

signals:
    void timeout();

public slots:
    void Update(uint32_t);

private:
    TimerManagement *timerMgmt;

    uint32_t time;
    bool state;

};

#endif // TIMER_H
