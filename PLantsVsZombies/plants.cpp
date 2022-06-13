#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>

#include "plants.h"
#include "mainwindow.h"


/*******************************************************************************************
                  植物管理

********************************************************************************************/
PlantManagement::PlantManagement(MainWindow *window)
{
    this->window = window;
    for (int i=0; i<MAP_COLUMN; ++i) {
        for (int j=0; j<MAP_LINE; ++j)
        {
            plantMap[i][j] = nullptr;
        }
    }
}

PlantManagement::~PlantManagement()
{

}

void PlantManagement::ShoverLoad()
{
    shover = new PlantsButton(window);
    shover->setFlat(true);
    shover->setStyleSheet("QPushButton{background-color:transparent;}");
    shover->setIcon(QIcon(QString(":/graphic/Screen/shovel.png")));    //设置图表
    shover->setIconSize(QSize(SLOVER_SOLT_WIDTH,SLOVER_SOLT_HEIGHT));
    shover->setGeometry(SLOVER_SOLT_POINTX, SLOVER_SOLT_POINTY, SLOVER_SOLT_WIDTH, SLOVER_SOLT_HEIGHT);     //设置位置，大小
    shover->setMouseTracking(true);
    shover->show();
    shoverAction = window->actionMgmt.Create(shover);

    // 检测选择铲子
    QObject::connect(shover, &PlantsButton::leftPressed, this, &PlantManagement::WipeOut);
}

void PlantManagement::WipeOut()
{
    QObject::disconnect(shover, &PlantsButton::leftPressed, 0, 0);  // 展暂时和鼠标点击断开
    shover->setCursor(Qt::BlankCursor);                             // 隐藏鼠标
    shoverAction->FollowMouse();                                    // 跟随鼠标动画

    // 左键铲除
    QObject::connect(shover, &PlantsButton::leftPressed, this, [=](){
        QPoint m = window->MousePostion();                      // 鼠标位置
        QPoint p = window->CoordToMapUnit(m);                     // 植物位置
        shover->move(SLOVER_SOLT_POINTX, SLOVER_SOLT_POINTY);   // 铲子归位
        shover->setCursor(Qt::ArrowCursor);                     // 显示鼠标
        shoverAction->Stop();

        // 在范围内且该位置有东西
        if (m.y()>75 && m.x()>50 && m.x()<800 && PlantMapState(p) == true)
        {
            delete plantMap[p.x()][p.y()];
            plantMap[p.x()][p.y()] = nullptr;

            qDebug() << "wipe out " << p;
        }

        // 恢复点击
        QObject::connect(shover, &PlantsButton::leftPressed, this, &PlantManagement::WipeOut);
    });

    // 右键取消
    QObject::connect(shover, &PlantsButton::rightPressed, this, [=](){
        QObject::disconnect(shover, &PlantsButton::rightPressed, 0, 0);

        shover->move(SLOVER_SOLT_POINTX, SLOVER_SOLT_POINTY);   // 铲子归位
        shover->setCursor(Qt::ArrowCursor);                     // 显示鼠标
        shoverAction->Stop();

        QObject::connect(shover, &PlantsButton::leftPressed, this, &PlantManagement::WipeOut);
    });
}


void PlantManagement::Place(PlantType type) 
{
    auto PlantCreate = [=]() -> Plants*
    {
        switch (type)
        {
            case PlantTypePeaShooter: return new PeaShooter(window);
            case PlantTypeSunflower: return new SunFlower(window);
            case PlantTypeWallNut: return new WallNut(window);
            case PlantTypeSnowPea: return new SnowPeaShooter(window);
            default: return new PeaShooter(window);
        }
    };

    Plants* plant = PlantCreate();
    // 确认放置植物
    QObject::connect(plant, &Plants::finishPlace, this, [=](QPoint p){
        plantMap[p.x()][p.y()] = plant;
        emit finishPlace();
    });
    // 取消放置
    QObject::connect(plant, &Plants::cancelPlace, this, [=](){
        delete plant;
        emit cancelPlace();
    });
}


void PlantManagement::Update()
{
    for (int i=0; i<MAP_COLUMN; ++i)
    {
        for (int j=0; j<MAP_LINE; ++j)
        {
            if (plantMap[i][j] != nullptr)
            {
                // 死亡检测
                if (plantMap[i][j]->isDie())
                {
                    delete plantMap[i][j];
                    plantMap[i][j] = nullptr;
                }
                else
                {
                    // 战斗检测
                    plantMap[i][j]->FireDetection();
                }
            }
        }
    }
}

void PlantManagement::PlantInjury(QPoint p, int atk)
{
    plantMap[p.x()][p.y()]->Injure(atk);
}


/*******************************************************************************************
                  植物父类

********************************************************************************************/
Plants::Plants(MainWindow *window)
{
    this->window = window;
    this->timer = new Timer(&window->timerMgmt); // 创建计时器
    this->model = new QLabel(window);
    this->gif = new QMovie();
    this->hint = new QLabel(window);
    this->button = new PlantsButton(window);
}

Plants::~Plants()
{
    delete model;      // 模型
    delete gif;        // 动图
    delete button;
    delete hint;
    delete timer;
}

void Plants::plantInit(int hp, BulletType type, int speed, int range)   //植物初始化
{
    this->hp        = hp;
    this->btype     = type;
    this->speed     = speed;
    this->range     = range;
}


void Plants::place()
{
    // 放置提示模型
    button->setFlat(true);
    button->setStyleSheet("QPushButton{background-color:transparent;}");
    button->setFixedSize(ModeSize());
    button->setIcon(QIcon(StaticModePath()));
    button->setIconSize(ModeSize());
    button->setMouseTracking(true);
    button->setCursor(Qt::BlankCursor);     // 隐藏鼠标
    button->show();
    // 放置位置提示
    hint->resize(ModeSize());
    hint->setPixmap(QPixmap(StaticModePath()));
    hint->setMouseTracking(true);
    hint->setCursor(Qt::BlankCursor);       // 隐藏鼠标
    hint->hide();
    // 放置位置显示标志
    QGraphicsOpacityEffect *o = new QGraphicsOpacityEffect;
    o->setOpacity(0.5);                // 半透明
    hint->setGraphicsEffect(o);        // 加载透明效果

    Action *placeAction = window->actionMgmt.Create(button);    // 创建动画
    placeAction->FollowMouse();                                 // 跟随鼠标移动

    // 模型跟随动画
    QObject::connect(placeAction, &Action::widgetPostion, this, [=](QPoint m) {
        QPoint p  = window->CoordToMapUnit(m);    // 种植位置
        QPoint lp = window->MapUnitCoord(p);      // 标签提示坐标

        // 种植提示标签动画
        hint->move(lp.x(), lp.y());         // 在植被上且没东西
        if (window->MapCoordValidity(m) && !window->plantMgmt.PlantMapState(p))
            hint->show();
        else
            hint->hide();
    });

    // 左键确认放置植物
    QObject::connect(button, &PlantsButton::leftPressed, this, [=](){
        button->blockSignals(true);

        QPoint m = window->MousePostion();          // 获取鼠标位置
        pos      = window->CoordToMapUnit(m);         // 种植位置
        if (!window->MapCoordValidity(m))
        {
            button->blockSignals(false);
            emit button->rightPressed();
            return;
        }
        if (window->plantMgmt.PlantMapState(pos))
        {
            placeAction->FollowMouse();             // 有东西了
            return;
        }
        Active();                                   // 动起来
        button->setCursor(Qt::ArrowCursor);         // 显示鼠标
        button->hide();
        hint->hide();
        placeAction->Finish();                      // 删除动画
        delete placeAction;

        emit finishPlace(pos);

        qDebug() << "place " << pos;
    });

    // 右键取消放置
    QObject::connect(button, &PlantsButton::rightPressed, this, [=](){
        button->blockSignals(true);

        button->setCursor(Qt::ArrowCursor);   // 显示鼠标
        button->hide();
        hint->hide();
        placeAction->Finish();
        delete placeAction;

        emit cancelPlace();

        qDebug() << "cancle place";
    });
}

void Plants::Active()
{
    QPoint p = window->MapUnitCoord(pos);
    model->setGeometry(p.x(), p.y(),
                       modeSize.width(),
                       modeSize.height());
    model->raise();
    model->show();
    gif->start();
    gif->setSpeed(GAME_SPEED);
}

void Plants::Injure(int atk)
{
    hp -= atk;
//    qDebug() << "injure" <<hp << atk;
}

// 默认情况下检测当前行
void Plants::FireDetection()
{
    QPoint p = window->CoordToMapUnit(pos);           // 种植坐标
    auto vec = &window->zombieMgmt.zombieVec[pos.y()];

    bool fire = false;

    for (auto z=vec->begin(); z!=vec->end(); ++z)
    {
        if ((*z)->Pos().x() >= p.x())
        {
            fire = true;
            break;
        }
    }

    if (fire && !timer->isActive())
        timer->Start();

    if (!fire && timer->isActive())
        timer->Stop();
}








