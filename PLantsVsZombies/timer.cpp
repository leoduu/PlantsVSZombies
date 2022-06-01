#include "timer.h"
#include "mainwindow.h"


TimerManagement::TimerManagement(QObject *parent)
{
    this->parent = parent;
    this->cnt = 0;
    timer.setParent(parent);
    timer.setInterval(UPDATE_RFREQUENCY*100/GAME_SPEED);
    timer.start();

    QObject::connect(&timer, &QTimer::timeout, this, [=](){
        cnt += 1;
        emit update(cnt);
    });
}

void TimerManagement::Add(Timer *timer)
{
    QObject::connect(this, &TimerManagement::update, timer, &Timer::Update);
}

void TimerManagement::Detach(Timer *timer)
{
    QObject::disconnect(this, &TimerManagement::update, timer, &Timer::Update);
}


Timer::Timer(TimerManagement *timerMgmt)
{
    this->timerMgmt = timerMgmt;
    this->state = false;
    this->timeoutCnt = 0;

    timerMgmt->Add(this);
}

Timer::~Timer()
{
    this->Stop();
    timerMgmt->Detach(this);

//    qDebug() << "~timer";
}

void Timer::Stop()
{
    state = false;
}

void Timer::setInterval(int time)
{
    this->time = time;
    this->NextPeriod();
}

void Timer::Start()
{
    state = true;
}

void Timer::Start(int time)
{
    this->time  = time;
    this->state = true;
    this->NextPeriod();
}

void Timer::NextPeriod()
{
    timeoutCnt = timerMgmt->timeCnt() + time/UPDATE_RFREQUENCY;
}

void Timer::Update(uint32_t cnt)
{
    if (state)
    {
        if (timeoutCnt<=cnt)
        {
            emit timeout();
            this->NextPeriod();
        }
    }
    else
        timeoutCnt++;
}

