#include <QSharedPointer>

#include "mainwindow.h"
#include "zombies.h"
#include "config.h"

const char* ACTION_PATH     = ":/graphic/Zombies/Zombie/";
const char* ATTACK_PATH     = ":/graphic/Zombies/Zombie/Attack.gif";
const char* ATTACK_PATH2    = ":/graphic/Zombies/Zombie/LostHeadAttack.gif";
const char* HEAD        = ":/graphic/Zombies/Zombie/Head.gif";
const char* LOST_HEAD   = ":/graphic/Zombies/Zombie/LostHead.gif";
const char* DIE         = ":/graphic/Zombies/Zombie/Die.gif";
const int ACTION_NUM    = 3;
const int HP            = 270;
const int ATTACK        = 100;
//const int SPEED         = 4000; 4s ~ 7s
const int WIDTH         = 166;
const int HEIGHT        = 144;

const int HP_LOST_HEAD  = HP/3;

//----------------------------------------------------------------------
//              普通僵尸
//
//------------------------------------------------------------------------
NormalZombie::NormalZombie(int line, MainWindow *window)
    : Zombies(line, window)
{
    zombieInit(HP, ATTACK, qrand()%3000+4000);
    pos = QPoint(WINDOW_WIDTH-100, 25+MAP_UNIT_HEIGHT*line);
    // 随机一种僵尸
    type = qrand()%ACTION_NUM;
    gif->setFileName(QString("%1Move%2.gif").arg(ACTION_PATH).arg(type));
    gif->start();
    label->setMovie(gif);
    label->setGeometry(pos.x(), pos.y(), WIDTH, HEIGHT);
    label->show();
    label->setMouseTracking(true);

    fight = false;

    action->Move(-14*MAP_UNIT_WIDTH, 0, speed*14);     //平动
    Sortie();
}

void NormalZombie::Fight(bool b)
{
    fight = b;

    if (fight)
    {
//        qDebug()<< "fight"<< MapUnitPos();
        gif->stop();
        if (state == ZombieStateLostHead)
            gif->setFileName(QString(ATTACK_PATH2));
        else
            gif->setFileName(QString(ATTACK_PATH));

        action->Stop();
        gif->start();
        atkTimer->Start(1000);
        QObject::connect(atkTimer, &Timer::timeout, this, [=](){
           if (state >= ZombieStateLostHead)
               window->plantMgmt.PlantInjury(MapUnitPos(), atk);
        });
    }
    else
    {
//        qDebug()<< "stop fight";
        atkTimer->Stop();
        gif->stop();
        if (state == ZombieStateLostHead)
            gif->setFileName(QString(LOST_HEAD));
        else
            gif->setFileName(QString("%1Move%2.gif").arg(ACTION_PATH).arg(type));
        gif->start();
        action->Resume();
        QObject::disconnect(atkTimer, &Timer::timeout, this, 0);
    }

}

void NormalZombie::Injury(int atk)
{
    this->hp -= atk;

//    qDebug() << "injury"<<hp <<atk <<state;

    if (hp < HP_LOST_HEAD && state == ZombieStateHealthy)     // 掉头
    {
        QMovie *m = new QMovie(QString(HEAD));
        QLabel *l = new QLabel(window);

        state = ZombieStateLostHead;
        gif->stop();
        if (isFight())
            gif->setFileName(QString(ATTACK_PATH2));
        else
            gif->setFileName(QString(LOST_HEAD));
        gif->start();

        l->setMovie(m);
        l->setGeometry(Pos().x()+66 , 25+MAP_UNIT_HEIGHT*line, 150, 186);
        m->start();
        l->show();

        modeTimer->Start(1300); // 掉头动画
        QObject::connect(modeTimer, &Timer::timeout, this, [=]() {
            QObject::disconnect(modeTimer, &Timer::timeout, this, 0);
            l->hide();
            delete m;
            delete l;
            modeTimer->Stop();
        });

        // 掉头流血
        bleedTimer->Start(500);
        QObject::connect(bleedTimer, &Timer::timeout, this, [=](){
            this->Injury(30);
        });
    }

    if (hp <= 0 && state == ZombieStateLostHead)  // 倒地
    {
        state = ZombieStateDie;
        bleedTimer->Stop();         // 停止流血
        gif->stop();
        gif->setFileName(QString(DIE));
        gif->start();
        label->resize(166,144);
        action->Move(-10, 0, 500);

        Timer *t = new Timer(&window->timerMgmt);
        t->Start(1300); // 倒地
        QObject::connect(t, &Timer::timeout, this, [=](){
            qDebug()<<"disappear";
            state = ZombieStateDisappear;
            label->hide();
            modeTimer->Stop();
            t->deleteLater();
        });        
    }
}

NormalZombie::~NormalZombie()
{

}
