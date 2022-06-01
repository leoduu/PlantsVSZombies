
#include <QMovie>
#include "plants.h"
#include "mainwindow.h"

const int HP            = 4000;
const int HP1           = 4000*2/3;
const int HP2           = 4000*1/3;

const int MODE_SIZEX    = 65;
const int MODE_SIZEY    = 73;

const char *DynamicMode2 = ":/graphic/Plants/WallNut/2.gif";
const char *DynamicMode3 = ":/graphic/Plants/WallNut/3.gif";


QString WallNut::StaticModePath()
{
    return QString(":/graphic/Plants/WallNut/0.gif");
}
QString WallNut::DynamicModePath()
{
    return QString(":/graphic/Plants/WallNut/1.gif");
}

WallNut::WallNut(MainWindow *window)
    : Plants(window)
{
    this->type = PlantTypeWallNut;
    this->hp = HP;
    this->state = 0;

    modeSize = QSize(MODE_SIZEX, MODE_SIZEY);
    gif->setFileName(WallNut::DynamicModePath());

    model->hide();
    model->setScaledContents(true);
    model->setMouseTracking(true);
    model->setMovie(gif);

    place();
}

void WallNut::Injure(int atk)
{
    hp -= atk;

    if (hp < HP1 && state == 0)
    {
        state = 1;
        gif->stop();
        gif->setFileName(DynamicMode2);
        gif->start();
    }

    if (hp < HP2 && state == 1)
    {
        state = 2;

        gif->stop();
        gif->setFileName(DynamicMode3);
        gif->start();
    }
}



