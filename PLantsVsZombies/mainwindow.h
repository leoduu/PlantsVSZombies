#ifndef  MainWindow_H
#define  MainWindow_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>

#include "action.h"
#include "timer.h"
#include "card.h"
#include "plants.h"
#include "bullet.h"
#include "zombies.h"
#include "config.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~ MainWindow() {};

    QPoint MousePostion() {return mousePostion;}
    QPoint MapUnitCoord(QPoint p) {return placePoint[p.x()][p.y()];}
    QPoint CoordToMapUnit(QPoint p) {return QPoint((p.x()-65)/MAP_UNIT_WIDTH, (p.y()-80)/MAP_UNIT_HEIGHT);}
    bool MapUnitValidity(QPoint p) {return (p.x()<MAP_COLUMN && p.x()>=0 && p.y()<MAP_LINE && p.y()>=0);}
    bool MapCoordValidity(QPoint p) {return (p.x()>50 && p.x()<850 && p.y()>75);}
    void SunUpdate(int n);

    class CardManagement   cardMgmt;    // 卡片对象
    class PlantManagement  plantMgmt;
    class BulletManagement bulletMgmt;
    class ZombieManagement zombieMgmt;
    class ActionManagement actionMgmt;
    class TimerManagement  timerMgmt;   // 全局刷新定时器

signals:
    void mapStop();             //地图结束移动 connect in MainWindow
    void startLoadCard();       //开始加载卡片 connect in MainWindow
    void timerUpdate();
    void leftPress();
    void rightPress();

private slots:
    void PlaceHandler(PlantType);       // 放置植物
    void FightDetecion();               // 战斗检测
    void SunGeneration();               // 阳光掉落
    void ZombieGeneration();            // 僵尸

private:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

    void LoadMap();             // 加载地图
    void ChoosePlants();        // 选择上场植物
    void StartFighting();       // 开始战斗

    QLabel *map;                // 地图
    QLabel *chooser;            // 上场植物槽
    QLabel *shoverSlot;         // 铲子槽
    QPushButton *shover;        // 铲子
    QLabel *sunLabel;           // 阳光数量展示
    int sunCount;               // 阳光计数

    QPoint mousePostion;        // 鼠标位置
    QPoint placePoint[MAP_COLUMN][MAP_LINE];
};


#endif //  MainWindow_H
