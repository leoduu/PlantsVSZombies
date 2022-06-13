
#include <QMovie>
#include <QDebug>
#include <QLabel>

#include "zombies.h"
#include "mainwindow.h"

ZombieManagement::ZombieManagement(MainWindow *window)
    : zombieVec{}, zombieFallVec{}
{
    this->window = window;
}

void ZombieManagement::Push(ZombieType type, int line)
{
    auto ZombieCreate = [=]() -> Zombies*
    {
        switch (type)
        {
        case ZombieTypeNormal: return new NormalZombie(line, window);
        default: return new NormalZombie(line, window);
        }
    };
    Zombies *zombie = ZombieCreate();
    zombieVec[line].push_back(zombie);
}


void ZombieManagement::Update()
{
    // 检测倒地僵尸并删除
    for (int i=0; i<MAP_LINE; ++i)
    {
        auto vec = &zombieFallVec[i];

        for (auto z=vec->begin(); z!=vec->end();)
        {
            if ((*z)->state == ZombieStateDisappear)
            {
                (*z)->deleteLater();
                z = vec->erase(z);
            }
            else
                ++z;
        }
    }

    // 检测正常僵尸并排序
    for (int i=0; i<MAP_LINE; ++i)
    {
        auto vec = &zombieVec[i];

        // 排序从右到左
        std::sort(vec->begin(), vec->end(), [](Zombies *z1, Zombies *z2)->bool {
            return z1->Pos().x() < z2->Pos().x();
        });

        for (auto z=vec->begin(); z!=vec->end();)
        {
            // 僵尸模型提升层次，后来居上
            (*z)->Raise();

            // 直接死亡
            if ((*z)->state == ZombieStateDisappear)
            {
                (*z)->deleteLater();
                z = vec->erase(z);
            }
            // 添加到倒下数组，等待倒地消失动作结束
            else if ((*z)->state == ZombieStateDie)
            {
                zombieFallVec[i].push_back(*z);
                z = vec->erase(z);
            }
            else
                ++z;
        }
    }

    // 攻击检查
    for (int i=0; i<MAP_LINE; ++i)
    {
        auto vec = &zombieVec[i];

        for (auto z=vec->begin(); z!=vec->end(); ++z)
        {
            QPoint p = (*z)->MapUnitPos();
            // 僵尸还在外面
            if (!window->MapUnitValidity(p))
                continue;

            bool b = window->plantMgmt.PlantMapState(p);

            if (b && !(*z)->isFight())
                (*z)->Fight(true);

            if (!b && (*z)->isFight())
                (*z)->Fight(false);
        }
    }
}


//-----------------------------------------------------------------------
//             僵尸父类
//
//------------------------------------------------------------------------

Zombies::Zombies(int line, MainWindow *window)
    : pos()
{
    this->window = window;
    this->line = line;
    this->effect = ZombieEffectNormal;

    atkTimer = new Timer(&window->timerMgmt);
    modeTimer = new Timer(&window->timerMgmt);
    bleedTimer = new Timer(&window->timerMgmt);
    effectTimer = new Timer(&window->timerMgmt);

    label = new QLabel(window);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);

    gif = new QMovie();
    gif->setSpeed(GAME_SPEED);

    action = window->actionMgmt.Create(label);
}

int num=0;

Zombies::~Zombies()
{
    delete label;
    delete gif;
    delete action;
    delete atkTimer;
    delete modeTimer;
    delete bleedTimer;
    delete effectTimer;

    qDebug() << "~Zombie" << ++num;
}

void Zombies::zombieInit(int hp, int atk, int speed)
{
    this->hp    = hp;
    this->atk   = atk;
    this->speed = speed;
    this->state = ZombieStateHealthy;
}

void Zombies::Sortie()
{
    QObject::connect(action, &Action::widgetPostion, this, [=](QPoint p) {
        pos = p;
        if (pos.x() < 0)
            this->state = ZombieStateDisappear;
    });

    QObject::connect(action, &Action::finish, this, [](){
        // 僵尸吃掉了你的脑子
    });
}

void Zombies::setEffect(ZombieEffect e)
{
    switch (e)
    {
        case ZombieEffectNormal: {
            gif->setSpeed(GAME_SPEED);
            action->SetSpeed(100);
            atkTimer->setInterval(500);

            QObject::disconnect(effectTimer, &Timer::timeout, this, 0);
        }
        case ZombieEffectSnow: {
            if (this->effect == ZombieEffectNormal)
            {
                gif->setSpeed(GAME_SPEED/2);
                action->SetSpeed(50);
                atkTimer->setInterval(1000/2);
                QObject::connect(effectTimer, &Timer::timeout, this, [=](){
                    QObject::disconnect(effectTimer, &Timer::timeout, this, 0);
                    setEffect(ZombieEffectNormal);
                });
            }
            // 冻10s
            effectTimer->Start(10000);
        }
    }

    this->effect = e;
}

void Zombies::Injury(int atk, BulletType)
{
    hp -= atk;
}



