//
// Created by 10593 on 2023/11/21.
//

#include <string.h>
#include <sys/time.h>
#include "busManagement.h"
#include "SC513_msgQueue.h"
#include "SC513_task.h"


/**
 * 发送单个rs422遥测
 * @param apid
 * @param commandCode
 */
void sendOneFrameTelemetryRS422(unsigned char apid, unsigned short commandCode);

//周期单位毫秒
unsigned long int periodTime = 0;
signed int readBusFile(){
    FILE *file = fopen("busManagement.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char binaryNumber[8];
    // 逐对读取数据，直到文件末尾
    unsigned char type,state,num_a,num_b,sendState,busPower1,busPower2,fast,slow;
    while (fscanf(file, "%7s,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", binaryNumber,&type,&state,&num_a,&num_b,&sendState,&busPower1,&busPower2,&fast,&slow) == 10) {
        // 成功读取一对数字
        unsigned int apid = strtoul(binaryNumber, NULL, 2);
        busState[apid][0] = (unsigned char )apid;
        busState[apid][1] = type;
        busState[apid][2] = state;
        busState[apid][3] = num_a;
        busState[apid][4] = num_b;
        busState[apid][5] = sendState;
        busState[apid][6] = busPower1;
        busState[apid][7] = busPower2;
        busState[apid][8] = fast;
        busState[apid][9] = slow;
     }

    for (int i = 0; i < 99; ++i) {
        if (busState[i][0] != 0 ){
//            LOG_INFO("index %d(%d,%d,%d,%d)",i, busState[i][0], busState[i][1],busState[i][2],busState[i][3]);
        }
    }
    // 检查是否因为文件末尾而退出循环
    if (feof(file)) {
        LOG_INFO("Reached end of file");
    } else {
        // 如果循环结束并非因为文件末尾，可能是由于格式错误等原因
        LOG_ERROR("Error reading from file");
    }
    return 0;
}


/**
 * 时间广播处理
 */
time_t currentTime;


void timeDisposal() {
    // 获取当前时间的时间戳
    unsigned int us = getCurrentTimeInMicroseconds();
    currentTime = time(NULL);
    time_t referenceTime = 1136073600; // 2006年1月1日0时0分0秒的时间戳
    time_t secondsSince2006 = currentTime - referenceTime; // 计算整秒计数值
//                printf("Full second value since 2006 %lld \n", secondsSince2006);
    can_frame_t canFrame;

    addr_frame addrFrame = {0}; // 初始化结构体为0

    // 设置各个标识字段的值
    addrFrame.priority = 0b01;
    addrFrame.sourceAddress = 0b000000;
    addrFrame.multicastFlag = 0b11;
    addrFrame.destinationAddress = 0b111111;
    addrFrame.frameFlag = 0b11;
    addrFrame.frameNumber = 0x0;
    addrFrame.functionCode = 0x1;

    // 调用组合字段的函数
    unsigned int addr = combineFields(&addrFrame);
    canFrame.addr = addr;

    canFrame.data[0] = 0x00;
    canFrame.data[1] = 0x00;
    canFrame.data[2] = (secondsSince2006 >> 24) & 0xFF;
    canFrame.data[3] = (secondsSince2006 >> 16) & 0xFF;
    canFrame.data[4] = (secondsSince2006 >> 8) & 0xFF;
    canFrame.data[5] = secondsSince2006 & 0xFF;
    canFrame.data[6] = (us >> 8) & 0xFF;;
    canFrame.data[7] = us & 0xFF;
    canFrame.data_len = sizeof(canFrame.data);

    unsigned char data[sizeof(char) + sizeof (can_frame_t)];
    data[0] = busState[0b111111][APID];
    memcpy(&data[1], &canFrame, sizeof(can_frame_t)); // 后面存放can_frame_t数据

    unsigned MQRes = 0;
    if (QueuePut(POSIX_PERIOD_MQ,data,sizeof(char) + sizeof (can_frame_t),&MQRes) == -1){
        LOG_ERROR("send periodMQ error!!");
    }
}

/**
 * 总线状态处理
 */
void busStateProcessing(){
    for (int i = 0; i < sizeof(busState) / sizeof(busState[0]); ++i){
        //总线中有APID是0,只对非0的做操作
        if (0!=busState[i][APID]){
            if (busState[i][A_BUS_NUM] > 1 && busState[i][B_BUS_NUM] > 1){
                busState[i][APID_STATE] = 2;
            } else if (busState[i][A_BUS_NUM] > 1){
                busState[i][APID_STATE] = 1;
            } else if (busState[i][B_BUS_NUM] > 1){
                busState[i][APID_STATE] = 0;
            }

            can_frame_t canFrame;
            //定义can帧中的addr
            addr_frame addrFrame = {0}; // 初始化结构体为0
            // 设置各个标识字段的值
            addrFrame.priority = 0b00;
            addrFrame.sourceAddress = 0x0;
            addrFrame.multicastFlag = 0b00;
            addrFrame.destinationAddress = busState[i][APID];
            addrFrame.frameFlag = 0b11;
            addrFrame.frameNumber = 0;
            addrFrame.functionCode = 0x3;

            //将该值组合
            unsigned int addr = combineFields(&addrFrame);
            canFrame.addr = addr;
            //指令码
            canFrame.data[0] = 0x00;
            canFrame.data[1] = 0x25;
            //单字节和校验
            unsigned char res = calculateOneChecksum(canFrame.data,2);
//            LOG_INFO("res = %d",res);
            canFrame.data[2] = res;
            canFrame.data_len = 3;
            for (int j = 3 ;j < 8; ++j){
                canFrame.data[j] = 0x00;
            }
            char data[sizeof(char) + sizeof (can_frame_t)];
            data[0] = busState[i][APID];
            memcpy(&data[1], &canFrame, sizeof(can_frame_t)); // 后面存放can_frame_t数据

//            char received_extra_char = data[0];
//            printf("received_extra_char = %d \n",received_extra_char);
//            can_frame_t received_frame;
//            memcpy(&received_frame, &data[1], sizeof(can_frame_t));
//            addr_frame addrFrame1 = convertToAddrFrame(received_frame.addr);
//            for (int j = 0; j < sizeof(received_frame.data) / sizeof(received_frame.data[0]); ++j){
//                printf("data = %d\n",received_frame.data[j]);
//            }

//            if (mq_send(immediateMQ, data, sizeof(char) + sizeof (can_frame_t),0)==-1){
//                perror("mq_send");
//                exit(EXIT_FAILURE);
//            }
        }
    }

}

void fastSlowTelemetry(unsigned long time){
    for (int i = 0; i < sizeof(busState) / sizeof(busState[0]); ++i){
        if (0!=busState[i][APID]&& busState[i][APID_TYPE] == BUS_CAN
        && busState[i][SEND_STATE] == SEND_STATE_BUS && (busState[i][BUS_POWER1] == 1 || busState[i][BUS_POWER2]  == 1)){
            if (busState[i][FAST_PERIOD] % time == 0){
                sendOneFrameTelemetryCan(busState[i][APID],RAPID_TELEMETRY);
            }
            if (busState[i][SLOW_PERIOD] % time == 0){
                sendOneFrameTelemetryCan(busState[i][APID],SLOW_TELEMETRY);
            }
        } else if (0!=busState[i][APID] && busState[i][APID_TYPE] == BUS_RS422
        && busState[i][SEND_STATE] == SEND_STATE_BUS && (busState[i][BUS_POWER1] == 1 || busState[i][BUS_POWER2]  == 1) ){

            if (busState[i][FAST_PERIOD] % time == 0){
                sendOneFrameTelemetryRS422(busState[i][APID],RAPID_TELEMETRY);
            }
            if (busState[i][SLOW_PERIOD] % time == 0){
                sendOneFrameTelemetryRS422(busState[i][APID],SLOW_TELEMETRY);
            }
        }
    }
}

void * busManagementfun(void *pVoid) {
    signed int res = readBusFile();
        if (res == -1){
            //读取总线文件失败
            printf("read bus file error!!\n");
        }
    while (CONDITION_TRUE){
//        //1秒1次的时间广播
//        //2秒1次的总线查询
//        //todo 缺少看门狗的使用

        //两个光纤陀螺快遥发送
//        sendOneFrameTelemetryRS422(GYRO_1_APID,RAPID_TELEMETRY);
//        sendOneFrameTelemetryRS422(GYRO_2_APID,RAPID_TELEMETRY);
        //一秒一次的快遥与时间广播
        if (periodTime % 1000 == 0) {
//            timeDisposal();
            fastSlowTelemetry(periodTime);
        }

        //两秒一次的总线查询
//        if (periodTime %2 == 0){
//            busStateProcessing();
//        }

//        //三秒一次的慢遥
//        if (periodTime %3000 == 0){
////            fastSlowTelemetry(SLOW_TELEMETRY);
//        }
////
        if (periodTime >= 12000) {
            periodTime = 0;
        }

        periodTime = periodTime +100;
        TaskDelay(100);;
    }
}



