
#include <QMovie>
#include "plants.h"
#include "mainwindow.h"

const BulletType BULLET = BulletTypeSunflower;
const int HP            = 300;
const int SPEED         = 24000;
const int RANGE         = 1;

const int MODE_SIZEX    = 64;
const int MODE_SIZEY    = 71;

QString SunFlower::StaticModePath()
{
    return QString(":/graphic/Plants/SunFlower/0.gif");
}
QString SunFlower::DynamicModePath()
{
    return QString(":/graphic/Plants/SunFlower/1.gif");
}

SunFlower::SunFlower(MainWindow *window)
    : Plants(window)
{
    fightState = true;
    type = PlantTypeSunflower;
    plantInit(HP, BULLET, SPEED, RANGE);

    // 加载模型
    modeSize = QSize(MODE_SIZEX, MODE_SIZEY);
    gif->setFileName(SunFlower::DynamicModePath());
    model->hide();
    model->setScaledContents(true);
    model->setMouseTracking(true);
    model->setMovie(gif);

    // 攻击速度
    timer->Start(speed);

    // 生产一个子弹
    QObject::connect(timer, &Timer::timeout, this, &Plants::Fire);

    place();
}

void SunFlower::Fire()        //植物攻击
{
    window->bulletMgmt.Push(pos, BULLET);
}

void SunFlower::FireDetection()
{
    fightState = true;
}


SunFlower::~SunFlower()
{

}

