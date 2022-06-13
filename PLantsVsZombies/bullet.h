#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QLayout>

#include "config.h"
#include "timer.h"

#define PEA_PATH            ":/graphic/Bullets/PeaNormal_0.png"
#define PEAICE_PATH         ":/graphic/Bullets/PeaIce_0.png"
#define PEA_ATTACK          20
#define PEA_SPEED           10


class Bullet;
class Sun;
class MainWindow;
class Action;

class BulletManagement : public QWidget
{
    Q_OBJECT

public:
    explicit BulletManagement(MainWindow *window=0);
    ~BulletManagement();
    void Push(QPoint, BulletType);
    void Update();

    QVector<Bullet *> bulletVec[MAP_LINE];
    QVector<Sun *> sunVec;

private:
    class MainWindow *window;
};


class Bullet : public QWidget
{
    Q_OBJECT

public:
    Bullet(BulletType type, QPoint p, MainWindow *window);
    ~Bullet();
    void Fire();
    QPoint Pos() {return QPoint(pos.x()-55, pos.y());}
    int X() {return pos.x();}

    QLabel *model;
    bool state;
    int attack;
    BulletType type;

private:
    class MainWindow *window;
    Action *action;
    QPoint pos;
    QSize size;
};



class Sun : public QPushButton
{
    Q_OBJECT

public:
    explicit Sun(QPoint p, BulletType type, MainWindow *window=0);
    ~Sun();

    bool state;

public slots:
    void Wait();
    void PickUp();

private:
    class MainWindow *window;
    Action *action;
    Timer *timer;
    QLabel *label;
    QMovie *gif;
    QVBoxLayout *layout;
};


#endif // BULLET_H
