#ifndef ACTION_H
#define ACTION_H

typedef enum {
    ActionTypeMove,
    ActionTypeMoveTo,
    ActionTypeFollowMouse,
    ActionTypeFadeAway,
} ActionType;

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QThread>

class Action;

class ActionManagement : public QObject
{
    Q_OBJECT

public:
    explicit ActionManagement(class MainWindow *window=0);
    ~ActionManagement();

    Action *Create(QWidget *);

signals:
    void update();
    void timerStop();
    void timerResume();

private:
    class MainWindow *window;
};


class Action : public QObject
{
    Q_OBJECT

public:
    Action(QWidget *);
    ~Action();

    void Stop();                            // 暂停
    void Resume();
    void Finish();
    void Move(int x, int y, int ms);        // 控件移动
    void MoveTo(int x, int y, int ms);      // 控件移动到坐标
    void FollowMouse();                     // 控件跟随鼠标
    void FadeAway(int ms);                  // 控件逐渐消失
    ActionType Type() {return type;}

signals:
    void widgetPostion(QPoint);
    void finish();              // 移动结束信号
    void start();               // 开始信号

public slots:
    // 一次移动
    void update();

private:
    void MoveInital(int x, int y, int ms);      // 位置数值初始化

    class MainWindow *window;
    QWidget *widget;
    ActionType type;    // 动画类型
    int startX;         // 初始X轴位置
    int startY;         // 初始Y轴位置
    int endX;           // 结束X轴位置
    int endY;           // 结束Y轴位置
    float preX;         // 一次移动X轴距离
    float preY;         // 一次移动Y轴距离
    float preO;         // 一次特效
    int totalNum;        // 移动次数
    int currNum;        // 当前移动次数
};

#endif // ACTION_H
