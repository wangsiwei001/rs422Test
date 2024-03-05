//#include <stdio.h>
#include <string.h>
//#include <stdint-gcc.h>
#include "SC513_synuart.h"
#include "SC513_task.h"
#include "SC513_can.h"
#include "mainTest.h"
#include "log.h"
#include "map.h"

#define KA 0x11 //KA线路
#define S 0x22 //S线路

#define S_LINK_INIT 0 //S初始化
#define KA_LINK_INIT 1 //KA初始化
#define FUNCTION_CODE_0X2 0X2

Map recombinationmMap;

unsigned kaBufferIndex = 0; //ka缓存下标
unsigned char kaBuffer[2048] = {0}; //ka接收缓存
unsigned char currentKaBuffer[2048] = {0}; //ka缓存�?
unsigned int currentKaBufferIndex = 0; //当前ka下标
unsigned int kaFrameLength = 0; //当前ka帧长�?
unsigned int kaOverLength = 34;

unsigned sBufferIndex = 0; //s缓存下标
unsigned char sBuffer[2048] = {0}; //s接收缓存
unsigned char currentSBuffer[2048] = {0}; //s缓存�?
unsigned int currentSBufferIndex = 0; //当前s下标
unsigned int sFrameLength = 0; //当前s帧长�?
unsigned int sOverLength = 34;
int number = 0;

typedef struct {
    unsigned short command : 16;       // 命令字 16 bits
    unsigned char commandType : 4; // 命令类型 4 bits
    unsigned char timeType : 2;   // 时间类型 2 bits
    unsigned char reserved1 : 2;  // 备用 1 2 bits
    unsigned int length : 21;   // 长度 21 bits
    unsigned char reserved2 : 3;  // 备用 2 3 bits
    unsigned char timeInfo[6];   // 时间信息 6 * 8 bits
} packet_MAPP;

typedef struct {
    unsigned char code:3;
    unsigned char priority : 2;//数据优先级
    unsigned char sourceAddress : 6;//源节点地址
    unsigned char multicastFlag : 2;//组播标识
    unsigned char destinationAddress : 6;//目的节点地址
    unsigned char frameFlag : 2;//帧序号标志
    unsigned char frameNumber : 6;//帧序号
    unsigned char functionCode : 5;//功能码
} addr_frame;

typedef struct {
    uint8_t versionNumber : 2;         // 版本号 (2 bit)
    uint8_t bypassFlag : 1;            // 旁路标识 (1 bit)
    uint8_t controlCommand : 1;        // 控制命令 (1 bits)
    uint8_t reserved : 2;              // 备用 (2 bits)
    uint8_t encryptionIdentifier : 2; // 明密标识 (2 bits)
    uint16_t spacecraftID : 16;          // 航天器识别字 (16 bits)
    uint8_t virtualChannelID : 6;     // 虚拟信道识别字 (6 bits)
    uint16_t frameLength : 10;           // 帧长 (10 bits)
    uint8_t frameSequenceNumber : 8;   // 帧序列号 (8 bits)
} frame_lead_Header;

typedef struct {
    uint8_t packageVersion : 3;         // 包版本号 (3 bits)
    uint8_t type : 1;                  // 类型 (1 bit)
    uint8_t auxiliaryHeaderFlag : 1;   // 副导头标识 (1 bit)
    uint16_t applicationProcessID : 11; // 应用过程标识 (11 bits)
    uint8_t sequenceFlag : 2;          // 序列标志 (2 bits)
    uint16_t packageSeqCount : 14;     // 包序列计数 (14 bits)
    uint16_t  packageLength : 16;            // 包长 (16 bits)
} packet_leading_head;

uint16_t calculateChecksum2(const uint8_t *data, size_t length) {
    uint16_t sum = 0;

    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }

    return ~sum; // 返回一个uint16_t类型的校验和
}

//typedef struct {
//    uint8_t versionNumber : 2;         // 版本号 (2 bit)
//    uint8_t bypassFlag : 1;            // 旁路标识 (1 bit)
//    uint8_t controlCommand : 1;        // 控制命令 (1 bits)
//    uint8_t reserved : 2;              // 备用 (2 bits)
//    uint8_t encryptionIdentifier : 2; // 明密标识 (2 bits)
//    uint16_t spacecraftID : 16;          // 航天器识别字 (16 bits)
//    uint8_t virtualChannelID : 6;     // 虚拟信道识别字 (6 bits)
//    uint16_t frameLength : 10;           // 帧长 (10 bits)
//    uint8_t frameSequenceNumber : 8;   // 帧序列号 (8 bits)
//} frame_lead_Header;

//typedef struct {
//    unsigned char code:3;
//    unsigned char priority : 2;//数据优先级
//    unsigned char sourceAddress : 6;//源节点地址
//    unsigned char multicastFlag : 2;//组播标识
//    unsigned char destinationAddress : 6;//目的节点地址
//    unsigned char frameFlag : 2;//帧序号标志
//    unsigned char frameNumber : 6;//帧序号
//    unsigned char functionCode : 5;//功能码
//} addr_frame;
//
//typedef struct {
//    unsigned int time;
//    unsigned int length;
//    can_frame_t canFrame[124];
//} can_frame_recombination;
//
//typedef struct {
//    unsigned short packageSize : 16; //长度
//    unsigned short command : 16 ; //指令码
//    unsigned char packageVersion : 3;// 包版本号 (3 bits)
//    unsigned char type : 1;// 类型 (1 bit)
//    unsigned char auxiliaryHeaderFlag : 1;// 副导头标识 (1 bit)
//    unsigned short applicationProcessID : 11;// 应用过程标识 (11 bits)
//    unsigned char sequenceFlag : 2;// 序列标志 (2 bits)
//    unsigned short packageSeqCount : 14;// 包序列计数 (14 bits)
//} can_res_epdu;
//
//can_res_epdu reconstruct_can_res(const unsigned char data [8]) {
//    can_res_epdu reconstructed;
//    reconstructed.packageSize = (data[0] << 8) | data[1];
//    LOG_INFO("reconstructed.packageSize = %d",reconstructed.packageSize);
//    reconstructed.command = (data[2] << 8) | data[3];
//    reconstructed.packageVersion = (data[4] >> 5) & 0x07;
//    reconstructed.type = (data[4] >> 4) & 0x01;
//    reconstructed.auxiliaryHeaderFlag = (data[4] >> 3) & 0x01;
//    reconstructed.applicationProcessID = ((data[4] & 0x07) << 8) | data[5];
//    reconstructed.sequenceFlag = (data[6] >> 6) & 0x03;
//    reconstructed.packageSeqCount = ((data[6] & 0x3F) << 8) | data[7];
//    return reconstructed;
//}
//
//unsigned short calculateChecksum2(const unsigned char *data, size_t length) {
//    unsigned short sum = 0;
//
//    for (size_t i = 0; i < length; ++i) {
//        sum += data[i];
//    }
//
//    return ~sum; // 返回一个unsigned short 类型的校验和
//}
//
//unsigned short convertToUnsignedShort(const unsigned char *array) {
//#ifdef BIG_ENDIAN_SYSTEM
//    return (array[0] << 8) | array[1];
//#else
////    return (array[1] << 8) | array[0];
//    return (array[0] << 8) | array[1];
//#endif
//}
//
//addr_frame convertToAddrFrame(unsigned int addr) {
//    addr_frame result;
//    unsigned char bytes[4];
//
//    bytes[0] = (addr >> 24) & 0xFF;
//    bytes[1] = (addr >> 16) & 0xFF;
//    bytes[2] = (addr >> 8) & 0xFF;
//    bytes[3] = addr & 0xFF;
//
//    /**
//     * TODO 未确认是否修改
//     */
////    result.priority = (bytes[0] >> 6) & 0x03;
////    result.sourceAddress = bytes[0] & 0x3F;
////    result.multicastFlag = (bytes[1] >> 6) & 0x03;
////    result.destinationAddress = bytes[1] & 0x3F;
////    result.frameFlag = (bytes[2] >> 6) & 0x03;
////    result.frameNumber = bytes[2] & 0x3F;
////    result.functionCode = (bytes[3] >> 3) & 0x1F;
//
//    result.code = (addr >> 29) & 0x07;
//    result.priority = (addr >> 27) & 0x03;
//    result.sourceAddress = (addr >> 21) & 0x3F;
//    result.multicastFlag = (addr >> 19) & 0x03;
//    result.destinationAddress = (addr >> 13) & 0x3F;
//    result.frameFlag = (addr >> 11) & 0x03;
//    result.frameNumber = (addr >> 5) & 0x3F;
//    result.functionCode = addr & 0x1F;
//    return result;
//}
//
//void functionCodeProcess(const can_frame_t *canFrame, const addr_frame *addrFrame, const can_frame_recombination *ptr) {
//    can_res_epdu canResEpdu = reconstruct_can_res(ptr->canFrame[0].data);
//    //帧总长度帧个数 * 8
//    unsigned short size = (ptr->length + 1) * 8;
//
//    if (ptr->time > 30 ) {
//        removeNode(&recombinationmMap, addrFrame->sourceAddress);
//        LOG_INFO("End frame insertion removeNode APID:%hhu", addrFrame->sourceAddress);
//    } else {
//        //开辟数组的长度是首帧字段长度-2，是因为长度是包括校验码以内的所有数据，但是校验码在这里不需要了
//        unsigned char frameArray[size * sizeof(unsigned char)];
//
//        int j = 0;
//        for (j = 0; j < ptr->length; ++j) {
//            //获取一帧
//            can_frame_t canFrame2 = ptr->canFrame[j];
//            //判断当前是第几帧
//            addr_frame addrFrame2 = convertToAddrFrame(canFrame2.addr);
////            LOG_INFO("addrFrame2.frameNumber %d",addrFrame2.frameNumber);
////            LOG_INFO("addrFrame2.frameFlag %d",addrFrame2.frameFlag);
//            if (addrFrame2.frameFlag == 0b00) {
//                //如果是中间帧
//                //-4是不要首帧的指令码与长度,如果索引已经超过63，并且帧序号小于10说明已经是下一个循环计数了
//                int index1 =j > 63 ? (addrFrame2.frameNumber * 8) + 512 : (addrFrame2.frameNumber * 8);
//                memcpy(&frameArray[index1], canFrame2.data, 8 * sizeof(unsigned char));
//            } else if (addrFrame2.frameFlag == 0b01) {
//                //如果是首帧的话从版本号开始拷贝
//                memcpy(frameArray, canFrame2.data, 8 * sizeof(unsigned char));
//            }
//        }
//
//        memcpy(&frameArray[ j * 8], canFrame->data,canFrame->data_len - 1);
//        LOG_INFO("end frame completion");
//        LOG_INFO("%lu", (canResEpdu.packageSize - 2) * sizeof(unsigned char));
//        unsigned int  sendLength = (j * 8) + canFrame->data_len - 1;
//        //是否有和校验待确认
//        unsigned short sunCheckRes = calculateChecksum2((unsigned char *) frameArray,
//                                                        sendLength * sizeof(unsigned char));
//        unsigned short sunCheckRes2 = canFrame->data[canFrame->data_len];
////        memCpy((unsigned char *) &sunCheckRes2, &canFrame->data[canFrame->data_len - 3], 2);
//        LOG_INFO("sunCheckRes == 0x%x", sunCheckRes);
//        LOG_INFO("sunCheckRes2 == 0x%x", sunCheckRes2);
//
//        if (sunCheckRes != sunCheckRes2){
//            LOG_ERROR("sunCheckRes or sunCheckRes2 error");
//            return;
//        }
////        //判断是否是姿轨控单机遥测信息
////        unsigned char apid = addrFrame->destinationAddress;
////
////        if (apid == STARTRACKER_1_APID_SIX || apid == STARTRACKER_2_APID_SIX || apid == STARTRACKER_3_APID_SIX) {
////            unsigned MQRes = 0;
////            if (QueuePut(POSIX_AOCS_MQ,frameArray,sendLength * sizeof(unsigned char),&MQRes) == -1){
////                LOG_ERROR("send AOCSMQ error!!");
////            }
////        }
////        unsigned MQRes = 0;
////        if (QueuePut(POSIX_RECEIVED_MQ,frameArray,sendLength * sizeof(unsigned char),&MQRes) == -1){
////            LOG_ERROR("send AOCSMQ error!!");
////        }
//
//        for (int i = 0; i < sendLength; ++i) {
//            printf("0x%x   ", frameArray[i]);
//        }
//
//
//        LOG_INFO("send msg receivedMQ completion");
//        removeNode(&recombinationmMap, addrFrame->sourceAddress);
//    }
//}
//
//void functionCode0x2Process(const can_frame_t *canFrame, const addr_frame *addrFrame, const can_frame_recombination *ptr) {
//    can_res_epdu canResEpdu = reconstruct_can_res(ptr->canFrame[0].data);
//    //帧总长度是校验和2字节+尾帧和校验1字节
//    unsigned short size = canResEpdu.packageSize + 3;
//    int canFrameSize = size / 8;
//    int remainder = size % 8;
//    if (remainder != 0) {
//        canFrameSize++;
//    }
//    if (ptr->time > 30 || ptr->length + 1 > canFrameSize) {
//        removeNode(&recombinationmMap, addrFrame->sourceAddress);
//        LOG_INFO("End frame insertion removeNode APID:%hhu", addrFrame->sourceAddress);
//    } else {
//        //开辟数组的长度是首帧字段长度-2，是因为长度是包括校验码以内的所有数据，但是校验码在这里不需要了
//        unsigned char frameArray[(canResEpdu.packageSize - 2) * sizeof(unsigned char)];
//        //如果是尾帧直接加入末尾
//        //要获取的长度是这个数组长度-1 -1是不需要最后一帧校验帧
//
//        unsigned short length = canFrame->data_len - 1;
//        unsigned int index = (addrFrame->frameNumber * 8) - 4;
//        //复合帧最后一帧的开始位置是总长度-2-最后一帧的长度
//        memcpy(&frameArray[(canResEpdu.packageSize - 2) - length], canFrame->data,
//               length * sizeof(unsigned char));
//        LOG_INFO("end frame completion");
//
//        can_frame_t addrFrame3;
//        for (int j = 0; j < ptr->length; ++j) {
//            //获取一帧
//            can_frame_t canFrame2 = ptr->canFrame[j];
//            //判断当前是第几帧
//            addr_frame addrFrame2 = convertToAddrFrame(canFrame2.addr);
////            LOG_INFO("addrFrame2.frameNumber %d",addrFrame2.frameNumber);
////            LOG_INFO("addrFrame2.frameFlag %d",addrFrame2.frameFlag);
//            if (addrFrame2.frameFlag == 0b00) {
//                //如果是中间帧
//                //-4是不要首帧的指令码与长度,如果索引已经超过63，并且帧序号小于10说明已经是下一个循环计数了
//                int index1 =j > 63 ? ((addrFrame2.frameNumber * 8) - 4) + 512 : (addrFrame2.frameNumber * 8) -4;
//                memcpy(&frameArray[index1], canFrame2.data, 8 * sizeof(unsigned char));
//            } else if (addrFrame2.frameFlag == 0b01) {
//                //如果是首帧的话从版本号开始拷贝
//                memcpy(frameArray, &canFrame2.data[4], 4 * sizeof(unsigned char));
//                memcpy(addrFrame3.data, canFrame2.data, 8);
//            }
//        }
//        LOG_INFO("%lu", (canResEpdu.packageSize - 2) * sizeof(unsigned char));
//        //判断和校验
//        unsigned short sunCheckRes = calculateChecksum2((unsigned char *) frameArray,
//                                                        (canResEpdu.packageSize - 2) * sizeof(unsigned char) - 2);
//        unsigned short sunCheckRes2 = convertToUnsignedShort(&canFrame->data[canFrame->data_len - 3]);
////        memCpy((unsigned char *) &sunCheckRes2, &canFrame->data[canFrame->data_len - 3], 2);
//        LOG_INFO("sunCheckRes == 0x%x", sunCheckRes);
//        LOG_INFO("sunCheckRes2 == 0x%x", sunCheckRes2);
//
////        if (sunCheckRes != sunCheckRes2){
////            LOG_ERROR("sunCheckRes or sunCheckRes2 error");
////            return;
////        }
//        //判断是否是姿轨控单机遥测信息
////        unsigned short apid = getAPID(frameArray, &apid);
////
////        if (apid == STARTRACKER_1_APID || apid == STARTRACKER_2_APID || apid == STARTRACKER_3_APID) {
////            unsigned MQRes = 0;
////            if (QueuePut(POSIX_AOCS_MQ,frameArray,(canResEpdu.packageSize - 2) * sizeof(unsigned char),&MQRes) == -1){
////                LOG_ERROR("send periodMQ error!!");
////            }
////        }
//
////        unsigned MQRes = 0;
////        if (QueuePut(POSIX_RECEIVED_MQ,frameArray,(canResEpdu.packageSize - 2) * sizeof(unsigned char),&MQRes) == -1){
////            LOG_ERROR("send msg receivedMQ error!!");
////        }
//
//        LOG_INFO("");
//        for (int i = 0; i < sizeof(addrFrame3.data) / sizeof(addrFrame3.data[0]); ++i) {
//            printf("0x%x   ", addrFrame3.data[i]);
//        }
//        for (int i = 0; i < (canResEpdu.packageSize - 2); ++i) {
//            printf("0x%x   ", frameArray[i]);
//        }
//        printf("0x%x   ", canFrame->data[length]);
//
//        LOG_INFO("send msg receivedMQ completion");
//        removeNode(&recombinationmMap, addrFrame->sourceAddress);
//    }
//}
//
//void headFrame(can_frame_t *canFrame, addr_frame *addrFrame) {
//    LOG_INFO("received head frame");
//    //如果是首帧
//    can_frame_recombination *canFrameRecombination;
//    canFrameRecombination = (can_frame_recombination *) malloc(sizeof(can_frame_recombination));
//    canFrameRecombination->time = 0;
//    canFrameRecombination->length = 1;
//    canFrameRecombination->canFrame[0] = (*canFrame);
//    ByteArray myValue = {canFrameRecombination, sizeof(can_frame_recombination)};
//    insert(&recombinationmMap, addrFrame->sourceAddress, myValue);
//    LOG_INFO("Insert head APID:%hhu", addrFrame->sourceAddress);
//}
//
//void bodyFrame(can_frame_t *canFrame, addr_frame *addrFrame) {//如果是中间帧
//    ByteArray byteArray = get(&recombinationmMap, addrFrame->sourceAddress);
//    if (byteArray.size > 0 && NULL != byteArray.data) {
//        can_frame_recombination *ptr = (can_frame_recombination *) byteArray.data;
//        if (ptr->time > 10) {;
//            removeNode(&recombinationmMap, addrFrame->sourceAddress);
//            LOG_INFO("Inter frame insertion removeNode APID:%hhu", addrFrame->sourceAddress);
//        } else {
//            ptr->time = 0;
//            ptr->canFrame[ptr->length] = (*canFrame);
//            ptr->length = ptr->length + 1;
//            LOG_INFO("Add one body frame APID:%hhu", addrFrame->sourceAddress);
//        }
//    }
//}
//
//void endFrame(can_frame_t *canFrame, addr_frame *addrFrame) {
//    ByteArray byteArray = get(&recombinationmMap, addrFrame->sourceAddress);
//    if (byteArray.size > 0 && NULL != byteArray.data) {
//        can_frame_recombination *ptr = (can_frame_recombination *) byteArray.data;
//        if (FUNCTION_CODE_0X2 == addrFrame->functionCode) {
//            functionCode0x2Process(canFrame, addrFrame, ptr);
//        } else{
//            functionCodeProcess(canFrame, addrFrame, ptr);
//        }
//    }
//}
//
////addr_frame convertToAddrFrame(unsigned int addr) {
////    addr_frame result;
////    unsigned char bytes[4];
////
////    bytes[0] = (addr >> 24) & 0xFF;
////    bytes[1] = (addr >> 16) & 0xFF;
////    bytes[2] = (addr >> 8) & 0xFF;
////    bytes[3] = addr & 0xFF;
////
////    /**
////     * TODO 未确认是否修改
////     */
//////    result.priority = (bytes[0] >> 6) & 0x03;
//////    result.sourceAddress = bytes[0] & 0x3F;
//////    result.multicastFlag = (bytes[1] >> 6) & 0x03;
//////    result.destinationAddress = bytes[1] & 0x3F;
//////    result.frameFlag = (bytes[2] >> 6) & 0x03;
//////    result.frameNumber = bytes[2] & 0x3F;
//////    result.functionCode = (bytes[3] >> 3) & 0x1F;
////
////    result.code = (addr >> 29) & 0x07;
////    result.priority = (addr >> 27) & 0x03;
////    result.sourceAddress = (addr >> 21) & 0x3F;
////    result.multicastFlag = (addr >> 19) & 0x03;
////    result.destinationAddress = (addr >> 13) & 0x3F;
////    result.frameFlag = (addr >> 11) & 0x03;
////    result.frameNumber = (addr >> 5) & 0x3F;
////    result.functionCode = addr & 0x1F;
////    return result;
////}
//
//void canFrameReceive() {
//
//    can_frame_t canFrame;
//    int res;
//    res = can_recv(1, &canFrame);
//    if (res ==0 && canFrame.data_len > 0) {
//        addr_frame addrFrame = convertToAddrFrame(canFrame.addr);
//        if (addrFrame.frameFlag == 0b11) {
//            //todo 判断是否是应答帧，放入接收报文队列
//
//            LOG_INFO("one frame");
//            for (int i = 0; i < canFrame.data_len; ++i){
//                printf("0x%x   ",canFrame.data[i]);
//                if (i == canFrame.data_len - 1){
//                    printf("\n");
//                }
//            }
//            LOG_INFO("");
//        } else if (addrFrame.frameFlag == 0b01) {
//            headFrame(&canFrame, &addrFrame);
//        } else if (addrFrame.frameFlag == 0b00) {
//            bodyFrame(&canFrame, &addrFrame);
//        } else if (addrFrame.frameFlag == 0b10) {
//            //如果是尾帧
//            LOG_INFO("end");
//            endFrame(&canFrame, &addrFrame);
//        }
//    }
//
//    res = can_recv(0, &canFrame);
//    if (res ==0 && canFrame.data_len > 0) {
//        addr_frame addrFrame = convertToAddrFrame(canFrame.addr);
//        if (addrFrame.frameFlag == 0b11) {
//            //todo 判断是否是应答帧，放入接收报文队列
//
//            LOG_INFO("one frame");
//            for (int i = 0; i < canFrame.data_len; ++i){
//                printf("0x%x   ",canFrame.data[i]);
//                if (i == canFrame.data_len - 1){
//                    printf("\n");
//                }
//            }
//            LOG_INFO("");
//        } else if (addrFrame.frameFlag == 0b01) {
//            headFrame(&canFrame, &addrFrame);
//        } else if (addrFrame.frameFlag == 0b00) {
//            bodyFrame(&canFrame, &addrFrame);
//        } else if (addrFrame.frameFlag == 0b10) {
//            //如果是尾帧
//            LOG_INFO("end");
//            endFrame(&canFrame, &addrFrame);
//        }
//    }
//}

//void KABufferProcess(const unsigned char * buffer,unsigned int length) {
//    //把最开始数据加入到buffer�?
//    memcpy(&currentKaBuffer[currentKaBufferIndex],buffer,length);
//    currentKaBufferIndex +=length;
//
//    //判断是否已有传送帧�?
//    //判断当前数据 + 当前Buffer长度大于或等于遥控传送帧长度
//    //帧长：等于传输帧全部字节数减 1；低速上行时遥控帧最大允许长度为1020 个字节，高速上行时，遥控帧�? 1020 字节固定长度，帧长固定填�?1111111011b�?
//    if (kaFrameLength != 0 && currentKaBufferIndex <= (kaFrameLength + 1)){
//        //获取剩余长度
//        unsigned int surplusLength = kaFrameLength - currentKaBufferIndex;
//        UINT16 crcValue = (UINT16) (((unsigned char) currentKaBuffer[kaFrameLength - 1] << 8) |
//                                    (unsigned char) currentKaBuffer[kaFrameLength]);
//        UINT16 crcRes  = crc16_ccitt_false(currentKaBuffer,kaFrameLength + 1);
//
//        if (crcRes == crcValue){
////            splitFlowTreatment(currentKaBuffer);
//            //从数据后开始获�?
//            int i;
//            for ( i = kaFrameLength; i < currentKaBufferIndex; ++i) {
//                //判断数据是否是帧�?
//                if ((i + 2 < currentKaBufferIndex) && (buffer[i] == 0x20 || buffer[i] == 0x23)
//                    && buffer[i + 1] == 0X00 && buffer[i + 2] == 0X03 ){
//                    //如果是帧头并且长度大�?4
//                    if ((currentKaBufferIndex - i) > 4){
//                        currentKaBufferIndex = currentKaBufferIndex - i;
//                        kaFrameLength = ((UINT16)(buffer[i + 3] & 0x03) << 8) | buffer[i + 4];
//                    } else{
//                        currentKaBufferIndex = currentKaBufferIndex - i;
//                        kaFrameLength = 0;
//                    }
//                    memcpy(currentKaBuffer,&currentKaBuffer[i],currentKaBufferIndex);
//                    return;
//                }
//            }
//            return;
//        } else{
//            int i;
//            for ( i = 2; i < currentKaBufferIndex; ++i) {
//                //判断数据是否是帧�?
//                if ((i + 2 < currentKaBufferIndex) && (buffer[i] == 0x20 || buffer[i] == 0x23)
//                    && buffer[i + 1] == 0X00 && buffer[i + 2] == 0X03 ){
////                     unsigned currentIndex = length - i;
//                    //如果是帧头并且长度大�?4
//                    if ((currentKaBufferIndex - i) > 4){
//                        currentKaBufferIndex = currentKaBufferIndex - i;
//                        kaFrameLength = ((UINT16)(buffer[i + 3] & 0x03) << 8) | buffer[i + 4];
//                    } else{
//                        currentKaBufferIndex = currentKaBufferIndex - i;
//                        kaFrameLength = 0;
//                    }
//                    memcpy(currentKaBuffer,&currentKaBuffer[i],currentKaBufferIndex);
//                    return;
//                }
//            }
//            return;
//        }
//    } else if (currentKaBufferIndex  >= 6){
//        kaFrameLength = ((UINT16)(buffer[3] & 0x03) << 8) | buffer[4];
//    }
//}



unsigned int combineFields(addr_frame *addrFrame) {

    /**
     * 未确认是否修改
     */
//    unsigned char bytes[4];
//    bytes[0] = (addrFrame->priority << 6) | addrFrame->sourceAddress;
//    bytes[1] = ((addrFrame->multicastFlag & 0x03) << 6) | (addrFrame->destinationAddress & 0x3F);
//    bytes[2] = ((addrFrame->frameFlag & 0x03) << 6) | (addrFrame->frameNumber & 0x3F);
//    bytes[3] = (addrFrame->functionCode & 0x1F) << 3;
//    int addr = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];

    unsigned char bytes[4];
    bytes[0] = (addrFrame->code << 5) | (addrFrame->priority << 3) | (addrFrame->sourceAddress >> 3);
    bytes[1] = ((addrFrame->sourceAddress & 0x07) << 5) | (addrFrame->multicastFlag << 3) | (addrFrame->destinationAddress >> 3);
    bytes[2] = ((addrFrame->destinationAddress & 0x07) << 5) | (addrFrame->frameFlag << 3) | (addrFrame->frameNumber >> 3);
    bytes[3] = ((addrFrame->frameNumber & 0x07) << 5) | addrFrame->functionCode;
    // 合并 unsigned char 数组为一个 int
    int addr = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    return addr;
}

unsigned char calculateOneChecksum(unsigned char *data, size_t length) {
    unsigned int sum = 0;

    // 遍历字节并累加
    int i;
    for (i = 0; i < length; ++i) {
        sum += data[i];
    }

    // 取累加和的低字节作为校验字节
    unsigned char checksum = (unsigned char)(sum & 0xFF);
    return checksum;
}

void oneCanFrameProcessing(const unsigned int length, unsigned char *msg2, const unsigned char *apid) {
    LOG_INFO("--------------------------");
    can_frame_t canFrame;
    unsigned char data[sizeof(char) + sizeof (can_frame_t)];
    data[0] = *apid;

    addr_frame addrFrame = {0}; // 初始化结构体为0
    // 设置各个标识字段的值

    addrFrame.priority = 0x0;
//    addrFrame.priority = 0b10;
    addrFrame.sourceAddress = 0x0;
    addrFrame.multicastFlag = 0x0;
    addrFrame.destinationAddress = *apid;
//    addrFrame.destinationAddress = 0b000001;
    addrFrame.frameFlag = 3;
    addrFrame.frameNumber = 0;
    addrFrame.functionCode = 0x3;
    if ((msg2[0] == 0x00 && msg2[1] == 0x5A) || (msg2[0] == 0x00 && msg2[1] == 0xAA)){
        addrFrame.functionCode = 0x1;
    } else if ((msg2[0] == 0x00 && msg2[1] == 0x00)){
        addrFrame.priority = 1;
    }

    //将该值组合
    unsigned int addr = combineFields(&addrFrame);
    canFrame.addr = addr;
    canFrame.data_len = length + 1;
    //单帧中canFrame.data[8]需要加一位校验和
    canFrame.data[length] = calculateOneChecksum(msg2, length);
    memcpy(canFrame.data, msg2, length);

    int i;
    for (i = (length + 1); i < 8; ++i) {
        canFrame.data[i] = 0x00;
    }
    memcpy(&data[1],&canFrame, sizeof(can_frame_t)); // 后面存放can_frame_t数据
//    if (mq_send(immediateMQ, (char *)data, sizeof(char) + sizeof (can_frame_t),0)==-1){
//        perror("mq_send");
//        exit(EXIT_FAILURE);
//    }
    can_send(0,&canFrame);
}

void splitFlowTreatment(const  unsigned char *buffer) {
    //按照位域结构体的字段将 recvbuf 中的字节还原为结构体
    unsigned int buffSize = 30;
    packet_MAPP packetMAPP;
    packetMAPP.command = (buffer[buffSize + 0] << 8) | buffer[buffSize + 1];
    packetMAPP.commandType = (buffer[buffSize + 2] >> 4) & 0xF;
    packetMAPP.timeType = (buffer[buffSize + 2] >> 2) & 0x3;
    packetMAPP.reserved1 = buffer[buffSize + 2] & 0x3;
    packetMAPP.length =
            ((unsigned int) (buffer[buffSize + 3] & 0xFF) << 13) | ((unsigned int) (buffer[buffSize + 4] & 0xFF) << 5) |
            ((unsigned int) (buffer[buffSize + 5] & 0xF8) >> 3);
    packetMAPP.length &= 0x1FFFFF;
    packetMAPP.reserved2 = buffer[buffSize + 5] & 0x7;

    buffSize = buffSize + 6;
    int j;
    for (j = 0; j < 6; ++j) {
        packetMAPP.timeInfo[j] = buffer[buffSize + j];
        buffSize++;
    }

    unsigned char msg2[packetMAPP.length];

    memcpy(msg2, &buffer[buffSize], packetMAPP.length);
    unsigned char apid = 96;
    oneCanFrameProcessing(packetMAPP.length, msg2, &apid);
}




UINT16 crc16_ccitt_false(UINT8 *data, UINT16 length)
{
    UINT8 i;
    UINT16 crc = 0xffff;        //Initial value
    while(length--)
    {
        crc ^= (UINT16)(*data++) << 8; // crc ^= (uint6_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void lowBufferSProcess(unsigned char *buffer, unsigned int length) {
    //把最开始数据加入到buffer�?
    memcpy(&currentSBuffer[currentSBufferIndex],buffer,length);
    currentSBufferIndex += length;

    //判断是否已有传送帧�?
    //判断当前数据 + 当前Buffer长度大于或等于遥控传送帧长度
    //这里是需要判断长度 + 1,并且还有结尾序列

    if (sFrameLength != 0 && currentSBufferIndex >= (sFrameLength + 1 + 2 + 8)){
        //获取剩余长度
        UINT16 crcValue = (UINT16) (((unsigned char) currentSBuffer[sFrameLength - 1] << 8) |
                                    (unsigned char) currentSBuffer[sFrameLength]);
        UINT16 crcRes  = crc16_ccitt_false(currentSBuffer,sFrameLength + 1);

//        if (crcRes == crcValue){
        if (1){
            splitFlowTreatment(currentSBuffer);
            int i;
            for (i = 0; i < sizeof(currentSBuffer) / sizeof(currentSBuffer[0]); ++i){
                printf("%d",currentSBuffer[i]);
            }
            sFrameLength = 0;
            sBufferIndex = 0;
            sOverLength = 34;
            return;
        } else{
            int i;
            for ( i = 2; i < currentSBufferIndex; ++i) {
                //判断数据是否是帧头
                if ((i + 1 < currentSBufferIndex) && currentSBuffer[i] == 0xEB && currentSBuffer[i + 1] == 0x90){
                    //如果是帧头并且长度大4
                    if ((currentSBufferIndex - i) > 4){
                        currentSBufferIndex = currentSBufferIndex - i;
                        kaFrameLength = ((UINT16)(buffer[i + 5] & 0x03) << 8) | buffer[i + 6];
                        sOverLength = sFrameLength - currentSBufferIndex;
                    } else{
                        currentSBufferIndex = currentSBufferIndex - i;
                        sFrameLength = 0;
                        sOverLength = 7 - currentSBufferIndex;
                    }
                    memcpy(currentSBuffer,&currentSBuffer[i],currentSBufferIndex);
                    return;
                }
            }
            currentSBufferIndex = 0;
            sFrameLength = 0;
            sOverLength = 34;
            return;
        }
    } else if (currentSBufferIndex  >= 7){

        if(currentSBuffer[0] == 0xEB && currentSBuffer[1] == 0x90){
            sFrameLength = ((UINT16)(currentSBuffer[5] & 0x03) << 8) | currentSBuffer[6];
            //获取帧的长度
            sOverLength = (sFrameLength + 1 + 2 + 8) - currentSBufferIndex;
            if (currentSBufferIndex >= (sFrameLength + 1 + 2 + 8)){
                //获取剩余长度
                UINT16 crcValue = (UINT16) (((unsigned char) currentSBuffer[sFrameLength - 1] << 8) |
                                            (unsigned char) currentSBuffer[sFrameLength]);
                UINT16 crcRes  = crc16_ccitt_false(currentSBuffer,sFrameLength + 1);

                if (crcRes == crcValue){
                    splitFlowTreatment(currentKaBuffer);
                    int i;
                    for (i = 0; i < sizeof(currentSBuffer) / sizeof(currentSBuffer[0]); ++i){
                        printf("%d",currentSBuffer[i]);
                    }
                    sFrameLength = 0;
                    if ((sFrameLength + 1 + 2 + 8) - currentSBufferIndex > 0){

                    } else{

                    }
                    return;
                } else{
                    int i;
                    for ( i = 2; i < currentSBufferIndex; ++i) {
                        //判断数据是否是帧头
                        if ((i + 1 < currentSBufferIndex) && currentSBuffer[i] == 0xEB && currentSBuffer[i + 1] == 0x90){
                            //如果是帧头并且长度大4
                            if ((currentSBufferIndex - i) > 4){
                                currentSBufferIndex = currentSBufferIndex - i;
                                kaFrameLength = ((UINT16)(buffer[i + 5] & 0x03) << 8) | buffer[i + 6];
                                sOverLength = sFrameLength - currentSBufferIndex;
                            } else{
                                currentSBufferIndex = currentSBufferIndex - i;
                                sFrameLength = 0;
                                sOverLength = 7 - currentSBufferIndex;
                            }
                            memcpy(currentSBuffer,&currentSBuffer[i],currentSBufferIndex);
                            return;
                        }
                    }
                    currentSBufferIndex = 0;
                    sFrameLength = 0;
                    sOverLength = 34;
                    return;
                }
            }
        } else {
            int i;
            for ( i = 2; i < currentSBufferIndex; ++i) {
                //判断数据是否是帧头
                if (((i + 1 ) < currentSBufferIndex) && currentSBuffer[i] == 0xEB && currentSBuffer[i + 1] == 0x90){
                    //如果是帧头并且长度大4
                    if ((currentSBufferIndex - i) >= 7){
                        currentSBufferIndex = currentSBufferIndex - i;
                        sFrameLength = ((UINT16)(buffer[i + 5] & 0x03) << 8) | buffer[i + 6];
                        sOverLength = (sFrameLength + 1 + 2 + 8) - currentSBufferIndex;
                    } else{
                        currentSBufferIndex = currentSBufferIndex - i;
                        sFrameLength = 0;
                        sOverLength = 7 - currentSBufferIndex;
                    }
                    memcpy(currentSBuffer,&currentSBuffer[i],currentSBufferIndex);
                    return;
                }
            }
            currentSBufferIndex = 0;
            sFrameLength = 0;
            sOverLength = 34;
            return;
        }
    }
}

void lowBufferKAProcess(unsigned char *buffer, unsigned int length) {
    //把最开始数据加入到buffer�?
    memcpy(&currentKaBuffer[currentKaBufferIndex],buffer,length);
    currentKaBufferIndex += length;

    //判断是否已有传送帧�?
    //判断当前数据 + 当前Buffer长度大于或等于遥控传送帧长度
    //这里是需要判断长度 + 1,并且还有结尾序列
    if (kaFrameLength != 0 && currentKaBufferIndex >= (kaFrameLength + 1 + 2 + 8)){
        //获取剩余长度
        UINT16 crcValue = (UINT16) (((unsigned char) currentKaBuffer[kaFrameLength - 1] << 8) |
                                    (unsigned char) currentKaBuffer[kaFrameLength]);
        UINT16 crcRes  = crc16_ccitt_false(currentKaBuffer,kaFrameLength + 1);

        if (crcRes == crcValue){

            splitFlowTreatment(currentKaBuffer);
            int i;
            for (i = 0; i < sizeof(currentKaBuffer) / sizeof(currentKaBuffer[0]); ++i){
                printf("%d",currentKaBuffer[i]);
            }
            kaFrameLength = 0;
            kaBufferIndex = 0;
            kaOverLength = 34;
            return;
        } else{
            int i;
            for ( i = 2; i < currentKaBufferIndex; ++i) {
                //判断数据是否是帧头
                if ((i + 1 < currentKaBufferIndex) && currentKaBuffer[i] == 0xEB && currentKaBuffer[i + 1] == 0x90){
                    //如果是帧头并且长度大4
                    if ((currentKaBufferIndex - i) > 4){
                        currentKaBufferIndex = currentKaBufferIndex - i;
                        kaFrameLength = ((UINT16)(buffer[i + 5] & 0x03) << 8) | buffer[i + 6];
                        kaOverLength = kaFrameLength - currentKaBufferIndex;
                    } else{
                        currentKaBufferIndex = currentKaBufferIndex - i;
                        kaFrameLength = 0;
                        kaOverLength = 7 - currentKaBufferIndex;
                    }
                    memcpy(currentKaBuffer,&currentKaBuffer[i],currentKaBufferIndex);
                    return;
                }
            }
            currentKaBufferIndex = 0;
            kaFrameLength = 0;
            kaOverLength = 34;
            return;
        }
    } else if (currentKaBufferIndex  >= 7){

        if(currentSBuffer[0] == 0xEB && currentKaBuffer[1] == 0x90){
            kaFrameLength = ((UINT16)(currentKaBuffer[5] & 0x03) << 8) | currentKaBuffer[6];
            //获取帧的长度
            kaOverLength = (kaFrameLength + 1 + 2 + 8) - currentKaBufferIndex;
        } else {
            int i;
            for ( i = 2; i < currentKaBufferIndex; ++i) {
                //判断数据是否是帧头
                if (((i + 1 ) < currentKaBufferIndex) && currentSBuffer[i] == 0xEB && currentSBuffer[i + 1] == 0x90){
                    //如果是帧头并且长度大4
                    if ((currentKaBufferIndex - i) >= 7){
                        currentKaBufferIndex = currentKaBufferIndex - i;
                        kaFrameLength = ((UINT16)(buffer[i + 5] & 0x03) << 8) | buffer[i + 6];
                        kaOverLength = (kaFrameLength + 1 + 2 + 8) - currentKaBufferIndex;
                    } else{
                        currentKaBufferIndex = currentKaBufferIndex - i;
                        kaFrameLength = 0;
                        kaOverLength = 7 - currentKaBufferIndex;
                    }
                    memcpy(currentKaBuffer,&currentKaBuffer[i],currentKaBufferIndex);
                    return;
                }
            }
            currentKaBufferIndex = 0;
            kaFrameLength = 0;
            kaOverLength = 34;
            return;
        }
    }
}


//void * taskSchedulingfun(void *pVoid) {
//    //同步422接收初始化
//    synuart_recv_init(S_LINK_INIT);
//    synuart_recv_init(KA_LINK_INIT);
//
//
//    while (1) {
//        unsigned int use_len = 0;
//        unsigned int res_len = 0;
//        unsigned int mq_res_len = 0;
//
//        synuart_get_rx_status(S, &use_len);
//
//        if (use_len > sOverLength) {
//            unsigned char buffer[sOverLength];
////            buffer[0] = 0x11;
//            synuart_recv(S, buffer, sOverLength, &res_len);
//            lowBufferSProcess(buffer, sOverLength);
//        }
//
//        synuart_get_rx_status(KA, &use_len);
//        if (use_len > kaFrameLength) {
//            unsigned char buffer[kaOverLength];
////            buffer[0] = 0x22;
//            synuart_recv(KA, buffer, kaOverLength, &res_len);
//            lowBufferKAProcess(buffer, kaOverLength);
//        }
//    }
//}

//void * taskSchedulingfun2(void *pVoid) {
//    //同步422发送初始化
//    synuart_send_init(S_LINK_INIT);
//    synuart_send_init(KA_LINK_INIT);
//    unsigned char sendbuf [1024] = {0};
//    sendbuf[0] = 0x1a;
//    sendbuf[1] = 0xCF;
//    sendbuf[2] = 0xFC;
//    sendbuf[3] = 0x1D;
//    unsigned int actLen = 0;
//    while (1){
//        synuart_send(0, sendbuf, 2048,&actLen);
//        synuart_send(1, sendbuf, 2048, &actLen);
//        TaskDelay(1000);
//    }
//}

int main() {
    unsigned char a [2] = {0xEB,0x90};
    lowBufferSProcess(a,2);

    unsigned char b [46] = {0X20,0X0,0X3,0X14,0X2d,0Xff,0X0,0X0,0X0,0X0,0X0,0X0,0X0,0X0,0X0,0X0,0X0,
                            0X0,0X0,0X0,0X0,0X0,0X10,0X90,0Xc0, 0X0,0X0,0Xd,0X0,0X1a,0X0,0X0,0X0,
                            0X10, 0X0, 0X0,0X0,0X0,0X0,0X0,0X0, 0X1a,0Xfe,0X4e,0X1a,0Xbd};
    lowBufferSProcess(b,46);


    unsigned char recvbuf [6] = {0};

//    frame_lead_Header frameLeadHeader;
//    frameLeadHeader.versionNumber = 0b00;
//    frameLeadHeader.bypassFlag = 0b1;
//    frameLeadHeader.controlCommand = 0b0;
//    frameLeadHeader.reserved = 0b00;
//    frameLeadHeader.encryptionIdentifier = 0b00;
//    frameLeadHeader.spacecraftID = 0b00000011;
//    frameLeadHeader.virtualChannelID = 0b000101;
//    frameLeadHeader.frameLength = 9;
//    frameLeadHeader.frameSequenceNumber = 0b11111111;
//
//    recvbuf[0] = (frameLeadHeader.versionNumber << 6) | (frameLeadHeader.bypassFlag << 5) | (frameLeadHeader.controlCommand << 4) | (frameLeadHeader.reserved << 2) | frameLeadHeader.encryptionIdentifier;
//    recvbuf[1] = (frameLeadHeader.spacecraftID >> 8);
//    recvbuf[2] = (uint8_t)(frameLeadHeader.spacecraftID & 0xFF);
//    recvbuf[3] = ((frameLeadHeader.virtualChannelID << 2) | (frameLeadHeader.frameLength >> 8));
//    recvbuf[4] = (uint8_t)(frameLeadHeader.frameLength & 0xFF);
//    recvbuf[5] = frameLeadHeader.frameSequenceNumber;

//    lowBufferSProcess(recvbuf,6);
//
//    unsigned char c [4] = {0xEB,0x90,0x50,0x60};
//    lowBufferSProcess(c,4);

    unsigned char d [8] = {0xC5,0xC5,0xC5,0xC5,0xC5,0xc5,0xc5,0x79};
    lowBufferSProcess(d,8);

    return 0;
}
