#ifndef CONFIG_H
#define CONFIG_H

// User Custom
#define GAME_SPEED          200     // 游戏倍速
#define AUTO_SUN_COLLECTION true    // 自动收集阳光



// WINDOW
#define WINDOW_WIDTH        900     // 窗口宽度
#define WINDOW_HEIGHT       600     // 窗口高度

// MAP
#define MAP_WIDTH           1400    // 地图宽度
#define MAP_HEIGHT          600     // 地图长度
#define MAP_LINE            5       // 行数
#define MAP_COLUMN          9       // 列数
#define MAP_UNIT_WIDTH      82      // 一格宽度
#define MAP_UNIT_HEIGHT     100     // 一格高度
#define CARD_SOLT_WIDTH     522     // 植物种植栏宽度
#define CARD_SOLT_HEIGHT    87      // 植物种植栏高度
#define CARD_CHOOSE_WIDTH   465     // 植物选择栏宽度
#define CARD_CHOOSE_HEIGHT  513     // 植物选择栏高度
#define SLOVER_SOLT_POINTX  522     // 铲子槽位置
#define SLOVER_SOLT_POINTY  0
#define SLOVER_SOLT_WIDTH   70      // 铲子槽长宽
#define SLOVER_SOLT_HEIGHT  74

#define UPDATE_RFREQUENCY   (1000 / 60)  //60Hz 刷新率

// Card
#define CARD_WIDTH          50      // 卡片长宽
#define CARD_HEIGHT         70
#define CARD_MOVE_TIME      150     // 卡牌移动时间
#define CARD_CLEARANCE      3       // 卡牌间隔
#define CARD_SLOT_NUMBER    8       // 上场植物总数
#define CARD_NUMBER         9       // 卡牌数量

typedef enum {
    PlantTypeEmpty = -1,
    PlantTypePeaShooter = 0,
    PlantTypeSunflower,
    PlantTypeWallNut,
    PlantTypeCherryBomb,
    PlantTypeSquash,
    PlantTypePotatoMine,
    PlantTypeSnowPea,
    PlantTypeChomper,
    PlantTypeRepeaterPea,
} PlantType;

typedef enum {
    BulletTypePea,
    BulletTypeSunScene,
    BulletTypeSunflower,
} BulletType;

typedef enum {
    ZombieTypeNormal,
} ZombieType;


#endif // CONFIG_H
