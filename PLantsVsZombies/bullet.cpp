#include <QLabel>
#include <QPushButton>
#include <QMovie>
#include <QDebug>
#include <QGraphicsOpacityEffect>

#include "bullet.h"
#include "mainwindow.h"

BulletManagement::BulletManagement(MainWindow *window)
    : bulletVec{}
{
    this->window = window;
}

BulletManagement::~BulletManagement()
{

}

void BulletManagement::Push(QPoint p, BulletType type)
{
    if (type == BulletTypeSunScene || type == BulletTypeSunflower)
    {
        sunVec.push_back(new Sun(p, type, window));
        return;
    }

    auto BulletCreate = [=]() -> Bullet*
    {
        switch (type)
        {
        case BulletTypePea: return new Bullet(QString(PEA_PATH), p, PEA_ATTACK, window);
        default: return new Bullet(QString(PEA_PATH), p, PEA_ATTACK, window);
        }
    };

    Bullet *b = BulletCreate();
    b->Fire();
    bulletVec[p.y()].push_back(b);
}

void BulletManagement::Update()
{
    for (auto s=sunVec.begin(); s!=sunVec.end();)
    {
        if ((*s)->state == false)
        {
            (*s)->deleteLater();
            s = sunVec.erase(s);
        }
        else
        {
            (*s)->raise();
            ++s;
        }
    }

    // 子弹和僵尸碰撞检测
    for (int i=0; i<MAP_LINE; ++i)
    {
        auto vecb = &bulletVec[i];
        auto vecz = &window->zombieMgmt.zombieVec[i];
        // 没有僵尸，退出当前行检测
        if (vecz->isEmpty()) continue;
        if (vecb->isEmpty()) continue;
        //  没有子弹

//        // 排序从右到左
//        std::sort(vecb->begin(), vecb->end(), [](Bullet *b1, Bullet *b2){
//            return b1->Pos().x() > b2->Pos().x();
//        });

        for (auto b=vecb->begin(); b!=vecb->end();)
        {

            if ((*b)->state == false)
            {
                (*b)->deleteLater();
                b = vecb->erase(b);
                continue;
            };

            int bx = (*b)->X()-55;

            for (auto z=vecz->begin(); z!=vecz->end(); ++z)
            {
                int zx = (*z)->Pos().x();
                if (bx >= zx && bx <= zx+80)
                {
                    (*z)->Injury((*b)->attack);
                    (*b)->state = false;
                    break;
    //                qDebug() << "zombie " << zombie->Pos();
    //                qDebug() << "bullet " << (*b)->Pos();
                }
            }

            ++b;
        }
    }
}


Bullet::Bullet(QString model_p, QPoint p, int atk, MainWindow *window)
    : pos()
{
    this->window = window;
    QPoint pp = window->MapUnitCoord(p);
    model = new QLabel(window);
    model->setScaledContents(true);
    model->setPixmap(QPixmap(model_p));
    model->setGeometry(pp.x()+43, pp.y(), 37, 34);
    model->setAttribute(Qt::WA_TransparentForMouseEvents);
    model->raise();
    model->show();

    this->pos = pp;
    this->attack = atk;
    this->state = true;
}

void Bullet::Fire()
{
    action = window->actionMgmt.Create(model);
    action->Move(1000, 0, 3000);

    QObject::connect(action, &Action::widgetPostion, this, [=](QPoint p){

        pos = p;
        if (p.x() > WINDOW_WIDTH) {
//            qDebug() << "out of map";
            attack = 0;
            state = false;
            action->Finish();
        }
    });
}

Bullet::~Bullet()
{
    delete model;
    delete action;
//    qDebug() << "~Bullet";
}


/*************************************************************
 *
 *          sun
 *
**************************************************************/

const char *SUN_MODE_PATH = "../graphics/Screen/Sun.gif";

Sun::Sun(QPoint p, BulletType type, MainWindow *window)
{
    this->window = window;
    this->setParent(window);
    this->state = true;
    this->timer = new Timer(&window->timerMgmt);

    QPoint pos = window->MapUnitCoord(p);
    gif = new QMovie(QString(SUN_MODE_PATH));
    gif->setSpeed(GAME_SPEED);
    gif->start();
    QGraphicsOpacityEffect *o = new QGraphicsOpacityEffect;
    o->setOpacity(0.9);                // 半透明
    label = new QLabel();
    label->setMovie(gif);
    label->setAlignment(Qt::AlignCenter);
    label->resize(90, 90);
    label->setGraphicsEffect(o);
    layout = new QVBoxLayout(this);
    layout->addWidget(label);

    this->setFlat(true);
    this->setStyleSheet("QPushButton{background-color:transparent;}");
    this->setMouseTracking(true);
    this->show();
    this->action = window->actionMgmt.Create(this);

    // 太阳花产生的
    if (type == BulletTypeSunflower)
    {
        this->setGeometry(pos.x(), pos.y(), 90, 90);
        action->MoveTo(pos.x()-(qrand()%60-30), pos.y(), 1000);
        QObject::connect(action, &Action::finish, this, &Sun::Wait);
    } // 场景掉落
    else if (type == BulletTypeSunScene)
    {
        this->setGeometry(pos.x(), 0, 90, 90);
        action->MoveTo(pos.x(), pos.y(), 500*(2+p.y()));
        QObject::connect(action, &Action::finish, this, &Sun::Wait);
    }

    // 点击捡阳光
    QObject::connect(this, &QPushButton::pressed, this, &Sun::PickUp);
}

Sun::~Sun()
{
    delete action;
    delete gif;
    delete label;
    delete layout;
    delete timer;
//    qDebug() << "~Sun";
}

void Sun::Wait()
{
    // 是否自动捡阳光
    if (AUTO_SUN_COLLECTION)
    {
        // 2s后不捡就自动捡
        timer->Start(1000);
        QObject::connect(timer, &Timer::timeout, this, &Sun::PickUp);
    }
    else
    {
        // 6s后不捡就消失
        timer->Start(6000);
        QObject::connect(timer, &Timer::timeout, this, [=](){
            this->hide();
            state = false;
        });
    }
}

void Sun::PickUp()
{
    QObject::disconnect(action, &Action::finish, this, 0);
    QObject::disconnect(this, &QPushButton::pressed, this, 0);

    action->MoveTo(5, 3, 1000);
    QObject::connect(action, &Action::finish, this, [=](){
        this->hide();
        state = false;
        window->SunUpdate(25);
    });
}








