//
// Created by 10593 on 2023/11/21.
//

#ifndef STARSOFTWARE_BUSMANAGEMENT_H
#define STARSOFTWARE_BUSMANAGEMENT_H
#include <stdint.h>


#define APID 0 //APID
#define APID_TYPE 1 //CAN OR RSS_422 OR  ETH
#define APID_STATE  2 //状态类型
#define A_BUS_NUM 3 // CAN A总线计数
#define B_BUS_NUM 4 // CAN 总线计数
#define SEND_STATE 5 // 发送状态
#define BUS_POWER1 6//主电源状态
#define BUS_POWER2 7//备电源状态
#define FAST_PERIOD 8 //快遥周期
#define SLOW_PERIOD 9 //慢遥周期
#define BUS_CAN 0 //类型CAN
#define BUS_RS422 1 //类型RS_422
#define BUS_ETH 2 //类型以太网
#define BUS_ERROR 2 //状态错误
#define SEND_STATE_NOT_BUS_ 0 //非总线管理发送
#define SEND_STATE_BUS 1 //总线管理发送
#define BUS_POWER_OFF 0x11 //电源关
#define BUS_POWER_ON 0x22 //电源开

/**
 *busSate:
 *下标是apid,每个数组
 *0:apid
 *1：总线类型（0：CAN,1：RS422,2：以太网）
 *2：当前总线（0是A总线，1是B总线，2是无总线）
 *3：A总线计数，
 *4：B总线计数
 *5：是否总线管理发送（0：否总线管理发送，1：是总线管理发送）
 *6：当前主设备加断电状态
 *7：当前备设备加断电状态
 *8：快遥周期
 *9：慢遥周期
 */
extern unsigned char busState[99][10];
void * busManagementfun(void *pVoid);

#endif //STARSOFTWARE_BUSMANAGEMENT_H
