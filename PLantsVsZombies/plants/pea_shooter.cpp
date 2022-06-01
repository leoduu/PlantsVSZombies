
#include <QMovie>
#include <qdebug.h>
#include "plants.h"
#include "mainwindow.h"

const BulletType BULLET = BulletTypePea;
const int HP            = 300;
const int SPEED         = 1400;
const int RANGE         = 1;

const int MODE_SIZEX    = 71;
const int MODE_SIZEY    = 71;

QString PeaShooter::StaticModePath()
{
    return QString("../graphics/Plants/Peashooter/0.gif");
}
QString PeaShooter::DynamicModePath()
{
    return QString("../graphics/Plants/Peashooter/1.gif");
}

PeaShooter::PeaShooter(MainWindow *window)
    : Plants(window)
{
    fightState = false;
    type = PlantTypePeaShooter;
    plantInit(HP, BULLET, SPEED, RANGE);

    // 加载模型
    modeSize = QSize(MODE_SIZEX, MODE_SIZEY);
    gif->setFileName(QString(PeaShooter::DynamicModePath()));
    model->hide();
    model->setScaledContents(true);
    model->setMouseTracking(true);
    model->setMovie(gif);

    // 攻击速度
    timer->setInterval(speed);

    // 生产一个子弹
    QObject::connect(timer, &Timer::timeout, this, &Plants::Fire);

    place();
}

void PeaShooter::Fire()        //植物攻击
{
    window->bulletMgmt.Push(pos, BULLET);
}


PeaShooter::~PeaShooter()
{

}
