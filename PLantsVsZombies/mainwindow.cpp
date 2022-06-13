#include "mainwindow.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QScrollBar>
#include <QtAlgorithms>
#include <stdlib.h>

MainWindow::MainWindow()
    : cardMgmt(this),
      plantMgmt(this),
      bulletMgmt(this),
      zombieMgmt(this),
      actionMgmt(this),
      timerMgmt(this)
{
    this->setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->setWindowTitle("Plants VS Zombies");
    this->setMouseTracking(true);                       //相应鼠标位置
    this->sunCount = 500;

    for (int i=0; i<MAP_COLUMN; i++)         //生成位置坐标
        for (int j=0; j<MAP_LINE; j++)
            MainWindow::placePoint[i][j] = QPoint(75+i*MAP_UNIT_WIDTH,
                                                  95+j*MAP_UNIT_HEIGHT);

    connect(&timerMgmt, &TimerManagement::update, this, [=](){
        emit timerUpdate();
    });

    LoadMap();
}

void MainWindow::SunUpdate(int n)
{
    sunCount += n;
    if (sunCount > 9999) sunCount = 9999;
    sunLabel->setText(QString::number(sunCount, 10));
    cardMgmt.Update(sunCount);
}

void MainWindow::LoadMap()
{
    // 加载地图
    map = new QLabel(this);                                //加载地图
    map->setPixmap(QPixmap(":/graphic/Map/Sunday.jpg"));   //加载地图
    map->setGeometry(0, 0, MAP_WIDTH, MAP_HEIGHT);
    map->setAlignment(Qt::AlignCenter);
    map->setMouseTracking(true);
    map->show();

    // 加载上场植物选择糟
    chooser = new QLabel(this);
    chooser->setPixmap(QPixmap(":/graphic/Screen/ChooserBackground.png"));
    chooser->setGeometry(0, 0, CARD_SOLT_WIDTH, CARD_SOLT_HEIGHT);
    chooser->setMouseTracking(true);
    chooser->hide();
    // 加载阳光计数
    sunLabel = new QLabel(this);
    sunLabel->setGeometry(13, 62, 52, 20);
    sunLabel->setStyleSheet("font-size:16px; font-family:Comic Sans MS");
    sunLabel->setAlignment(Qt::AlignCenter);
    sunLabel->setText(QString::number(sunCount, 10));
    sunLabel->raise();
    sunLabel->hide();
    // 加载植物种植选择糟
    QLabel *cardSolt = new QLabel(this);
    cardSolt->setPixmap(QPixmap(":/graphic/Screen/PanelBackground.png"));
    cardSolt->setGeometry(0, CARD_SOLT_HEIGHT, CARD_CHOOSE_WIDTH, CARD_CHOOSE_HEIGHT);
    cardSolt->hide();
    // 开始按钮
    QPushButton *startButton = new QPushButton(this);                   // 创建开始战斗按钮
    startButton->setIcon(QIcon(":/graphic/Screen/StartButton.png"));  // 设置图表
    startButton->setIconSize(QSize(154, 37));                           // 设置图片大小
    startButton->setGeometry(157, 547, 154, 37);                        // 设置位置，大小
    startButton->hide();

    // 稍作等待
    Action *mapLoad = actionMgmt.Create(map);
    mapLoad->Move(0, 0, 1500);
    // 地图初始移动
    Action *mapMove = actionMgmt.Create(map);
    QObject::connect(mapLoad, &Action::finish, this, [=](){     //地图停止移动
        mapMove->Move(WINDOW_WIDTH-MAP_WIDTH, 0, 1000);
        delete mapLoad;
    });

    // 地图移动结束，进行植物选择
    QObject::connect(mapMove, &Action::finish, this, [=](){     //地图停止移动
        chooser->show();
        sunLabel->show();
        cardSolt->show();
        emit startLoadCard();   //开始选择卡片
        delete mapMove;
    });

    //选上场植物
    QObject::connect(this, &MainWindow::startLoadCard, this, [=](){

        // 开始进行卡片选择
        cardMgmt.CardShow();

        // 选卡结束后显示开始战斗按钮
        QObject::connect(&cardMgmt, &CardManagement::canStartFighting, this, [=](){
            startButton->show(); // 一大波僵尸即将来临
        });

        // 上场植物卡满选下来一张后隐藏开始战斗按钮
        QObject::connect(&cardMgmt, &CardManagement::cannotStartFighting, this, [=](){
            startButton->hide();
        });

        // 点击开始战斗
        QObject::connect(startButton, &QPushButton::pressed, this, [=](){
            cardSolt->deleteLater();
            startButton->deleteLater();
            StartFighting();
        });
    });

}


void MainWindow::StartFighting()         //开始战斗
{
    qDebug() << "3 2 1 准备！";

    // 卡牌切换到战斗模式
    cardMgmt.FightingMode();
    cardMgmt.Update(sunCount);

    // 地图缩回来
    Action* mapBack = actionMgmt.Create(map);
    mapBack->MoveTo(-180, 0, 1000);

    // 加载铲子槽
    shoverSlot = new QLabel(this);
    shoverSlot->setPixmap(QPixmap(":/graphic/Screen/shovelSlot.png"));
    shoverSlot->setGeometry(522, 0, 84, 87);
    shoverSlot->show();
    plantMgmt.ShoverLoad();

    // 检查植物放置
    QObject::connect(&cardMgmt, &CardManagement::switchPlant, this, &MainWindow::PlaceHandler);

    // 战斗检查（子弹打击，僵尸攻击死亡，植物攻击死亡）
    QObject::connect(this, &MainWindow::timerUpdate, this, &MainWindow::FightDetecion);

    // 开启阳光掉落
    SunGeneration();

    // 生成僵尸
    ZombieGeneration();
}


void MainWindow::PlaceHandler(PlantType type)    //放置植物
{
    int n = (int) type;
    int cost = cardMgmt.GetPlantCost(n);

    if (sunCount < cost)
        return;

    cardMgmt.SetCardState(n, CardState::CardHide);    //隐藏图标

    // 放置植物
    plantMgmt.Place(type);

    // 完成后卡片恢复
    QObject::connect(&plantMgmt, &PlantManagement::finishPlace, this, [=](){
        QObject::disconnect(&plantMgmt, 0, 0, 0);
        cardMgmt.SetCardState(n, CardState::CardCooldown);
        SunUpdate(-1*cost);
    });

    QObject::connect(&plantMgmt, &PlantManagement::cancelPlace, this, [=](){
        QObject::disconnect(&plantMgmt, 0, 0, 0);
        cardMgmt.SetCardState(n, CardState::CardDisplay);
    });
}

void MainWindow::SunGeneration()
{
    Timer *timer = new Timer(&this->timerMgmt);
    timer->Start(8000);

    QObject::connect(timer, &Timer::timeout, this, [=](){
        int x = qrand() % MAP_LINE;
        int y = qrand() & MAP_COLUMN;
        bulletMgmt.Push(QPoint(x, y), BulletTypeSunScene);
    });
}

void MainWindow::ZombieGeneration()
{
    static int num = 0;

    if (1)
    {
        QObject::connect(this, &MainWindow::leftPress, this, [=](){
            QPoint p = mousePostion;
            int n = (p.y()-80)/100;
            zombieMgmt.Push(ZombieTypeNormal, n);

            qDebug() << "new zombie" << n << ++num;
        });
    }
    else
    {
        Timer *timer = new Timer(&this->timerMgmt);
        timer->Start(5000);
        QObject::connect(timer, &Timer::timeout, this, [=](){
            int n = qrand() % MAP_LINE;
            zombieMgmt.Push(ZombieTypeNormal, n);
            qDebug() << "new zombie" << n << ++num;
        });

        QObject::connect(this, &MainWindow::rightPress, this, [=](){
            if (timer->isActive())
                timer->Stop();
            else
                timer->Start();
        });
    }
}

void MainWindow::FightDetecion()
{

    // 僵尸更新
    zombieMgmt.Update();

    // 植物更新
    plantMgmt.Update();

    // 子弹碰撞检测
    bulletMgmt.Update();
}


void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    this->mousePostion = e->pos();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        qDebug() << "---- left ----";
        emit leftPress();
    }
    else if (e->button() == Qt::RightButton)
    {
        qDebug() << "---- right ---";
        emit rightPress();
    }
}




