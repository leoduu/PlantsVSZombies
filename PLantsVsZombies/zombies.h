#ifndef ZOMBIES_H
#define ZOMBIES_H

#include <QWidget>
#include <QMovie>
#include <QLabel>

#include "timer.h"
#include "config.h"

typedef enum {
    ZombieStateDisappear,
    ZombieStateDie,
    ZombieStateLostHead,
    ZombieStateArmor0,
    ZombieStateArmor1,
    ZombieStateHealthy,
} ZombieState;

class MainWindow;
class Zombies;
class Action;

class ZombieManagement : public QObject
{
    Q_OBJECT

public:
    explicit ZombieManagement(MainWindow *window=0);
    ~ZombieManagement() {}
    void Push(ZombieType, int line);
    void Update();

    QVector<Zombies *> zombieVec[MAP_LINE];         // 正常僵尸
    QVector<Zombies *> zombieFallVec[MAP_LINE];     // 倒地僵尸

private:
    MainWindow *window;
};

//--------  僵尸父类  --------//
class Zombies : public QWidget
{
    Q_OBJECT

public:
    explicit Zombies(int line, MainWindow *window=0);
    ~Zombies();
    virtual void Fight(bool) {};        // 攻击
    virtual void Injury(int);       // 僵尸被攻击
    virtual int Attack() {return atk;}
    virtual bool isFight() {return fight;}

    void Raise() {label->raise();}

    QPoint Pos() {return pos;}
    QPoint MapUnitPos() {return QPoint((pos.x()+35)/MAP_UNIT_WIDTH, line);}

    ZombieState state;

signals:
    void die();

protected:
    virtual void Sortie();              // 出击
    void zombieInit(int hp, int atk, int speed); // 僵尸数值初始化

    MainWindow *window;
    QPoint pos;
    QSize modeSize;
    QLabel *label;
    Action *action;
    QMovie *gif;
    Timer *atkTimer;
    Timer *modeTimer;
    Timer *bleedTimer;
    bool fight;
    int line;
    int frameNum;
    int hp;
    int atk;
    int speed;
};


//------  普通僵尸 ------//
class NormalZombie : public Zombies
{
    Q_OBJECT

public:
    explicit NormalZombie(int line, MainWindow *window=0);
    ~NormalZombie();
    void Fight(bool) override;           //攻击
    void Injury(int) override;       //僵尸被攻击

private:
    int type;
};

#endif // ZOMBIES_H
