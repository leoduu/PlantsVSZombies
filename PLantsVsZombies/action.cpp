#include <QDebug>
#include <QWidget>
#include <QMouseEvent>
#include <QObject>
#include <QGraphicsOpacityEffect>

#include "action.h"
#include "config.h"
#include "mainwindow.h"

ActionManagement::ActionManagement(MainWindow *window)
{
    this->window = window;
}


ActionManagement::~ActionManagement()
{

}

Action *ActionManagement::Create(QWidget *w)
{

    Action *action = new Action(w);

    QObject::connect(action, &Action::start, this, [=](){
       QObject::connect(window, &MainWindow::timerUpdate, action, &Action::update);    // 连接移动信号
    });


    QObject::connect(action, &Action::finish, this, [=](){
       QObject::disconnect(window, &MainWindow::timerUpdate, action, 0);
    });

    return action;
}


Action::Action(QWidget *w)
{
    this->widget = w;
}

Action::~Action()
{
//    qDebug() << "~Action";
}

void Action::Stop()
{
    emit finish();
}

void Action::Resume()
{
    emit start();
}

void Action::Finish()
{
    emit finish();
}

void Action::update()
{
//    qDebug() << "update" << type;

    if (type == ActionTypeFollowMouse)
    {
        QPoint p = ((MainWindow *)widget->parent())->MousePostion();    // 获取鼠标位置
        QSize s = widget->size();
        widget->move(p.x()-s.width()/2, p.y()-s.height()/2);            // 控件位于鼠标中心
        widget->raise();
        emit widgetPostion(p);
    }
    else if (type == ActionTypeFadeAway)
    {
        QGraphicsOpacityEffect *o = (QGraphicsOpacityEffect *)widget->graphicsEffect();
        o->setOpacity(1-preO);
        widget->setGraphicsEffect(o);   // 加载透明效果

        if (currNum == totalNum) {       // 消失
            delete o;
            Finish();
        }
    }
    else
    {
        ++currNum;
        widget->move( (int)(startX+preX*currNum), (int)(startY+ preY*currNum));

        if (currNum == totalNum) {       //移动完成
            widget->move(endX, endY);   //修正误差
            Finish();
        }
        else
            emit widgetPostion(widget->pos());
    }
}

//根据方向移动
void Action::Move(int x, int y, int ms)
{
    type = ActionTypeMove;              // 动作类型
    MoveInital(x+widget->pos().x(), y+widget->pos().y(), ms);
}


//根据结束位置移动
void Action::MoveTo(int x, int y, int ms)
{
    type = ActionTypeMoveTo;            // 动作类型
    MoveInital(x, y, ms);
}


//控件跟随鼠标
void Action::FollowMouse()
{
    type = ActionTypeFollowMouse;
    QPoint p = ((MainWindow *)widget->parent())->MousePostion();      // 获取鼠标位置
    widget->raise();
    widget->move(p.x()-25, p.y()-35);   // 控件位于鼠标中心
    emit start();
}

// 渐变消失
void Action::FadeAway(int ms)
{
    type = ActionTypeFadeAway;
    totalNum = ms / UPDATE_RFREQUENCY;
    preO = (float)1 / totalNum;     // 单次移动Y轴
    QGraphicsOpacityEffect *o = new QGraphicsOpacityEffect;
    o->setOpacity(1);
    widget->setGraphicsEffect(o);
    emit start();
}

void Action::MoveInital(int x, int y, int ms)
{
    this->startX = widget->pos().x();                   // 起始坐标X
    this->startY = widget->pos().y();                   // 起始坐标Y
    this->endX = x;                                     // 结束坐标X
    this->endY = y;                                     // 结束坐标Y
    this->totalNum = ms / UPDATE_RFREQUENCY;            // 移动次数
    this->currNum = 0;                                  // 当前移动此次数
    this->preX = (float)(endX - startX) / totalNum;     // 单次移动X轴
    this->preY = (float)(endY - startY) / totalNum;     // 单次移动Y轴
    emit start();

//    qDebug()<<"param: start("<<startX<<','<<startY<<") "
//            <<"end("<<endX<<','<<endY<<") "
//            <<"pre("<<preX<<','<<preY<<") "
//            <<"timer("<<time<<") ";
}







