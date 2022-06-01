#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include "action.h"
#include "config.h"

typedef enum {
    CardLeisure,
    CardHide,
    CardDisplay,
    CardCooldown,
    CardInitialCooldown,
    CardLackofMomey,
} CardState;

class CardManagement : public QObject
{
    Q_OBJECT

public:
    explicit CardManagement(class MainWindow *window=0);
    ~CardManagement();
    void CardShow();        // 卡片显示
    void LoadCard();        // 准备模式，加载卡片
    void FightingMode();    // 战斗模式
    void Update(int);
    void SetCardState(int n, CardState state);
    PlantType GetSlotPlant(int n) {return slotPlants[n];}
    CardState GetCardState(int n) {return cardState[n];}
    int GetPlantCost(int n) {return cost[n];}

signals:
    void canStartFighting();        // 加载开始战斗按钮
    void cannotStartFighting();     // 隐藏开始战斗按钮
    void switchPlant(PlantType);    // 选取植物

private:
    class MainWindow *window;                       // 主窗口地址
    QPushButton *cardButton[CARD_NUMBER];           // 按钮数组
    QSize cardXY[CARD_NUMBER];                      // 按钮初始坐标
    Action *action[CARD_NUMBER];                    // 动画类数组
    QString path[CARD_NUMBER];                      // 图片地址数组
    int currentPlantNumber;                         // 当前卡牌槽卡片数量
    int cardLocation[CARD_NUMBER];                  // 当前卡片位置
    CardState cardState[CARD_NUMBER];               // 卡片状态数组
    int cooldown[CARD_NUMBER];                      // 植物冷却
    int cost[CARD_NUMBER];                          // 植物花费
    PlantType slotPlants[CARD_SLOT_NUMBER];         // 卡牌槽对应植物
    QLabel *shadow[CARD_SLOT_NUMBER];               // 卡牌阴影
};

#endif // CARD_H


