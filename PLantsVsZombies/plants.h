#ifndef PLANTS_H
#define PLANTS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>

#include "timer.h"
#include "config.h"

class Action;
class MainWindow;

class PlantManagement : public QObject
{
    Q_OBJECT

public:
    explicit PlantManagement(MainWindow *window);
    ~PlantManagement();

    void ShoverLoad();
    void Place(PlantType);
    bool PlantMapState(QPoint p) {return plantMap[p.x()][p.y()]==nullptr?false:true;}
    void Update();
    void PlantInjury(QPoint p, int atk);

signals:
    void finishPlace();
    void cancelPlace();
    void plantFire(PlantType, QPoint);

public slots:
    void WipeOut();

private:
    class Plants *plantMap[MAP_COLUMN][MAP_LINE];
    class PlantsButton *shover;
    class Action *shoverAction;
    class MainWindow *window;
};


/*********************************
             响应右键按钮

*********************************/
class PlantsButton : public QPushButton
{
    Q_OBJECT

public:
    PlantsButton(QWidget *p) : QPushButton(p) {
        leftPress = false;
        rightPress = false;
    }
    ~PlantsButton() {}

signals:
    void leftPressed();
    void leftClicked();
    void rightPressed();
    void rightClicked();

protected:
     void mousePressEvent(QMouseEvent *e) override {
         if (e->button() == Qt::LeftButton) {
             leftPress = true;
             emit leftPressed();
         } else if (e->button() == Qt::RightButton) {
             rightPress = true;
             emit rightPressed();
         }
     }
     void mouseReleaseEvent(QMouseEvent *e) override {
         if (e->button() == Qt::LeftButton && leftPress) {
             leftPress = false;
             emit leftClicked();
         } else if (e->button() == Qt::RightButton && rightPress) {
             rightPress = false;
             emit rightClicked();
         }
     }
private:
     bool leftPress;
     bool rightPress;
};



/*********************************
             植物父类

*********************************/
class Plants : public QObject
{
    Q_OBJECT

public:
    explicit Plants(MainWindow *window=0);
    ~Plants();

    void place();
    virtual void Active();          // 动
    virtual void Injure(int);       // 受伤
    virtual void FireDetection();   // 开火检测

    virtual QString StaticModePath() {return QString();}
    virtual QString DynamicModePath() {return QString();}
    QSize ModeSize() {return this->modeSize;}
    bool isDie() {return hp<=0;}

signals:
    void finishPlace(QPoint);
    void cancelPlace();
    void fireABullet(QPoint, BulletType);

public slots:
    virtual void Fire() {};                    // 攻击模式

protected:
    MainWindow *window;
    QSize modeSize;             // 模型大小
    QLabel *model;              // 模型
    QMovie *gif;                // 动图
    PlantsButton *button;       // 放置按钮
    QLabel *hint;               // 放置提示

    QPoint pos;                 // 地图位置
    PlantType type;             // 植物类型
    BulletType btype;           // 攻击方式
    int hp;                     // 生命值
    int speed;                  // 攻击速度
    int range;                  // 射程
    int cost;                   // 花费
    int state;                  // 当前状态
    bool fightState;            // 攻击状态

    Timer *timer;               // 内置定时器

    void plantInit(int hp, BulletType btype, int speed, int range);   // 植物数值初始化
};

/*********************************
             豌豆射手

*********************************/

class PeaShooter : public Plants
{
    Q_OBJECT

public:
    explicit PeaShooter(MainWindow *window=0);
    ~PeaShooter();
    QString StaticModePath() override;
    QString DynamicModePath() override;

public slots:
    void Fire() override;                   // 攻击

private:

};


/*********************************
             向日葵

*********************************/
class SunFlower : public Plants
{
    Q_OBJECT

public:
    explicit SunFlower(MainWindow *window=0);
    ~SunFlower();
    void FireDetection() override;
    QString StaticModePath() override;
    QString DynamicModePath() override;

public slots:
    void Fire() override;                   // 攻击

};

/*********************************
             坚果墙

*********************************/
class WallNut : public Plants
{
    Q_OBJECT

public:
    explicit WallNut(MainWindow *window=0);

    QString StaticModePath() override;
    QString DynamicModePath() override;

    void Injure(int) override;       // 受伤
};

/*********************************
             冰豌豆射手

*********************************/
class SnowPeaShooter : public Plants
{
    Q_OBJECT

public:
    explicit SnowPeaShooter(MainWindow *window=0);
    ~SnowPeaShooter();
    QString StaticModePath() override;
    QString DynamicModePath() override;

public slots:
    void Fire() override;                   // 攻击
private:

};

///*********************************
//             樱桃炸弹

//*********************************/
//class CherryBomb : public Plants
//{
//    Q_OBJECT

//public:
//    explicit CherryBomb(MainWindow *parent = nullptr);
//    ~CherryBomb();
//    virtual void fight();       //攻击模式
//    virtual void injured();     //受伤

//private:

//protected:
//};

///*********************************
//             倭瓜

//*********************************/
//class Squash : public Plants
//{
//    Q_OBJECT

//public:
//    explicit Squash(MainWindow *parent = nullptr);
//    ~Squash();
//    virtual void fight();       //攻击模式
//    virtual void injured();     //受伤

//private:

//protected:
//};

///*********************************
//             土豆地雷

//*********************************/
//class PotatoMine : public Plants
//{
//    Q_OBJECT

//public:
//    explicit PotatoMine(MainWindow *parent = nullptr);
//    ~PotatoMine();
//    virtual void fight();       //攻击模式
//    virtual void injured();     //受伤

//private:

//protected:
//};



///*********************************
//             食人花

//*********************************/
//class Chomper : public Plants
//{
//    Q_OBJECT

//public:
//    explicit Chomper(MainWindow *parent = nullptr);
//    ~Chomper();
//    virtual void fight();       //攻击模式
//    virtual void injured();     //受伤

//private:

//protected:
//};

///*********************************
//             豌豆爸爸

//*********************************/
//class RepeaterPea : public PeaShooter
//{
//    Q_OBJECT

//public:
//    explicit RepeaterPea(MainWindow *parent = nullptr);
//    ~RepeaterPea();
//    virtual void fight();       //攻击模式

//private:

//protected:
//};

#endif // PLANTS_H

/*********************** 植物数据 *******************************
 *
 * NUM    NMAE     HP  ATK  SPEED  RANGE  COOLING  COST   frame
 *  0   豌豆射手   300   20   1400   Line    7500    100    13
 *
 *
 *
 *
 *
 *
 *
 ***************************************************************/
















