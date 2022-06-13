#include <QGraphicsOpacityEffect>

#include "card.h"
#include "mainwindow.h"

const char *PATH = ":/graphic/Cards/";

const int CooldownFast   = 7500;
const int CooldownMiddle = 30000;
const int CooldownSlow   = 50000;
const int CooldownInitialMiddle = 20000;
const int CooldownInitialSlow   = 35000;

const int PeaShooterCost = 100;
const int SunFlowerCost  = 50;
const int WallNutCost    = 50;


CardManagement::CardManagement(MainWindow *window)
{
    this->window = window;
    this->LoadCard();
    this->currentPlantNumber = 0;
    for (int i=0; i<CARD_SLOT_NUMBER; i++)
    {
        this->slotPlants[i]= PlantTypeEmpty; //初始化空

        shadow[i] = new QLabel(window);
        QGraphicsOpacityEffect *o = new QGraphicsOpacityEffect;
        o->setOpacity(0.5);                // 半透明
        shadow[i]->setGeometry(0,0, CARD_WIDTH, CARD_HEIGHT);
        shadow[i]->setStyleSheet("QLabel{background-color:rgb(0,0,0);}");
        shadow[i]->setGraphicsEffect(o);
        shadow[i]->setGeometry(82+i*(CARD_WIDTH+CARD_CLEARANCE+1), 7, CARD_WIDTH, CARD_HEIGHT);
    }
}


//加载植物卡片
void CardManagement::LoadCard()
{
    path[PlantTypePeaShooter]   = QString("%1card_peashooter.png").arg(PATH);       //豌豆射手
    path[PlantTypeSunflower]    = QString("%1card_sunflower.png").arg(PATH);        //向日葵
    path[PlantTypeWallNut]      = QString("%1card_wallnut.png").arg(PATH);          //坚果墙
    path[PlantTypeSnowPea]      = QString("%1card_snowpea.png").arg(PATH);          //冰豌豆射手
    path[PlantTypePotatoMine]   = QString("%1card_potatomine.png").arg(PATH);       //土豆地雷
    path[PlantTypeCherryBomb]   = QString("%1card_cherrybomb.png").arg(PATH);       //樱桃炸弹
    path[PlantTypeChomper]      = QString("%1card_chomper.png").arg(PATH);          //食人花
    path[PlantTypeSquash]       = QString("%1card_squash.png").arg(PATH);           //倭瓜
    path[PlantTypeRepeaterPea]  = QString("%1card_repeaterpea.png").arg(PATH);      //豌豆爸爸

    cooldown[PlantTypePeaShooter]    = CooldownFast;
    cooldown[PlantTypeSunflower]     = CooldownFast;
    cooldown[PlantTypeWallNut]       = CooldownMiddle;
    cooldown[PlantTypeSnowPea]       = CooldownFast;
    cooldown[PlantTypePotatoMine]    = CooldownFast;
    cooldown[PlantTypeCherryBomb]    = CooldownFast;
    cooldown[PlantTypeChomper]       = CooldownFast;
    cooldown[PlantTypeSquash]        = CooldownFast;
    cooldown[PlantTypeRepeaterPea]   = CooldownFast;

    cost[PlantTypePeaShooter]   = PeaShooterCost;
    cost[PlantTypeSunflower]    = SunFlowerCost;
    cost[PlantTypeWallNut]      = WallNutCost;
    cost[PlantTypeSnowPea]      = 0;
    cost[PlantTypePotatoMine]   = 0;
    cost[PlantTypeCherryBomb]   = 0;
    cost[PlantTypeChomper]      = 0;
    cost[PlantTypeSquash]       = 0;
    cost[PlantTypeRepeaterPea]  = 0;

    // 创建模型，放到指定位置
    for (int i=0; i<CARD_NUMBER; i++)
    {
        cardButton[i] = new QPushButton(window);                    //创建按钮
        cardButton[i]->setIcon(QIcon(path[i]));                     //设置图表
        cardButton[i]->setIconSize(QSize(CARD_WIDTH, CARD_HEIGHT));
        cardXY[i] = QSize(25+(i%8)*(CARD_WIDTH+CARD_CLEARANCE), 128+i/8*(CARD_HEIGHT+CARD_CLEARANCE));
        cardButton[i]->setGeometry(cardXY[i].width(), cardXY[i].height(), CARD_WIDTH, CARD_HEIGHT);     //设置位置，大小
        cardButton[i]->hide();
    }
}

//准备战斗时选择上场的植物
void CardManagement::CardShow()
{
    int i;
    // 所有卡牌初始化
    for (i=0; i<CARD_NUMBER; i++)
    {
        //置顶并显示
        cardButton[i]->raise();                 // 卡片置顶
        cardButton[i]->show();                  // 显示卡片
        cardButton[i]->setMouseTracking(true);
        cardState[i] = CardState::CardLeisure;
        cardLocation[i] = -1;
    }

    // 卡牌绑定选取动作
    for (i=0; i<CARD_NUMBER; i++)
    {
        action[i] = window->actionMgmt.Create(cardButton[i]);    // 创建动画对象

        QObject::connect(cardButton[i], &QPushButton::clicked, this, [=]() // 给每个卡片设置点击选取事件
        {
            cardButton[i]->blockSignals(true);              // 信号阻塞防止再点击
            if (cardState[i] == CardState::CardLeisure)     // 判断卡片是否已经选择
            {   // 位于待选取植物栏
                if (this->currentPlantNumber < CARD_SLOT_NUMBER)
                {
                    qDebug() << "pick"<<i;
                    int j=-1;
                    while (slotPlants[++j]!=PlantTypeEmpty);        // 找到空位
                    cardState[i] = CardState::CardDisplay;          // 卡片状态变更
                    cardLocation[i] = j;                            // 记录当前位置
                    action[i]->MoveTo(82+j*(CARD_WIDTH+CARD_CLEARANCE+1), 7, CARD_MOVE_TIME); //移去战斗植物栏
                    slotPlants[j] = (PlantType)i;                   // 记录当前位置的植物
                    ++currentPlantNumber;                           // 加一已选择卡牌的数量
//                    if (currentPlantNumber == CARD_SLOT_NUMBER)     // 够数了可以点击开始战斗
                    if (currentPlantNumber > 0)                     // 有就可以点击开始战斗
                        emit canStartFighting();
                }
            }
            else if (cardState[i] == CardState::CardDisplay)
            {   // 已经被选取，点击放回待选区
                qDebug() << "cancel pick"<<i;
//                if (currentPlantNumber == CARD_SLOT_NUMBER)         // 如果开始按钮亮了取消
                if (currentPlantNumber == 0)         // 如果开始按钮亮了取消
                    emit cannotStartFighting();
                action[i]->MoveTo(cardXY[i].width(), cardXY[i].height(), CARD_MOVE_TIME); //移回待选栏
                slotPlants[cardLocation[i]] = PlantTypeEmpty;       // 清除当前卡槽植物
                cardState[i] = CardState::CardLeisure;              // 卡片状态变更
                --currentPlantNumber;                               // 已选择卡牌数量减一
            }
            cardButton[i]->blockSignals(false);                     // 信号阻塞解除，允许点击
        });
    }
}

void CardManagement::SetCardState(int n, CardState state)   //n 植物类型  0隐藏  1显示  2冷却
{
    cardState[n] = state;
    bool initialCooldown = false;

    switch (state)
    {
    case CardHide: {
        cardButton[n]->hide();
        shadow[cardLocation[n]]->hide();
        break;
    }
    case CardDisplay: {
        cardButton[n]->show();
        shadow[cardLocation[n]]->hide();
        break;
    }
    case CardInitialCooldown:
        initialCooldown = true;
    case CardCooldown: {
        cardButton[n]->show();
        cardButton[n]->blockSignals(true);
        shadow[cardLocation[n]]->hide();

        QLabel *l1 = new QLabel(window);
        l1->setGeometry(82+cardLocation[n]*(CARD_WIDTH+CARD_CLEARANCE+1), 7, CARD_WIDTH, CARD_HEIGHT);
        l1->show();
        QLabel *l2 = new QLabel(l1);
        QGraphicsOpacityEffect *o = new QGraphicsOpacityEffect;
        o->setOpacity(0.5);                // 半透明
        l2->setGeometry(0,0, CARD_WIDTH, CARD_HEIGHT);
        l2->setStyleSheet("QLabel{background-color:rgb(0,0,0);}");
        l2->setGraphicsEffect(o);
        l2->show();

        int cd = cooldown[n];
        if (initialCooldown)
        {
            if (cd == CooldownMiddle)
                cd = CooldownInitialMiddle;
            else if (cd == CooldownSlow)
                cd = CooldownInitialSlow;
        }

        Action *action = window->actionMgmt.Create(l2);
        action->Move(0, CARD_HEIGHT, cd);
        connect(action, &Action::finish, this, [=](){
            cardButton[n]->blockSignals(false);
            delete action;
            delete l1;
        });
        break;
    }
    case CardLackofMomey: {
        shadow[cardLocation[n]]->raise();
        shadow[cardLocation[n]]->show();
        break;
    }
    default: break;
    }
}

void CardManagement::FightingMode()
{
    // 所有卡片断开连接
    for (int i=0; i<CARD_NUMBER; ++i)
    {
        if (GetCardState(i) == CardState::CardLeisure)
            cardButton[i]->hide();
        disconnect(cardButton[i], &QPushButton::clicked, 0,0);
    }

    // 上场卡片建立新连接
    for (int i=0; i<CARD_SLOT_NUMBER; i++)
    {
        int k = GetSlotPlant(i);
        if (k<0) continue;

        // 初始冷却
        if (cooldown[k] != CooldownFast)
            SetCardState(k, CardInitialCooldown);

        connect(cardButton[k], &QPushButton::clicked, this, [=](){
            emit switchPlant((PlantType)k);
        });
    }
}

void CardManagement::Update(int sunCount)
{
    for (int i=0; i<CARD_SLOT_NUMBER; i++)
    {
        int k = GetSlotPlant(i);
        if (k<0) continue;

        if (cost[k]>sunCount && GetCardState(k)!=CardLackofMomey)
        {
//            qDebug() << "LackofMomey" << k;
            SetCardState(k, CardLackofMomey);
        }
        else if (cost[k]<=sunCount && GetCardState(k)==CardLackofMomey)
        {
//            qDebug() << "Display" << k;
            SetCardState(k, CardDisplay);
        }
    }
}

CardManagement::~CardManagement()
{
//    qDebug()<< "~card";
}





