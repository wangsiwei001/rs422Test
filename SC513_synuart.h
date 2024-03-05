//////////////////////////////////////////////////////////////////////////  
///    COPYRIGHT NOTICE  
///    Copyright (c) 2015,513  
///    All rights reserved.  
///  
/// @file   SC513_synuart.h
/// @brief  同步UART驱动头文件
///         
///  
///  
///  
///  
/// @version 1.00
/// @author  gengzh
/// @date    2023/10/24
///  
///  修订说明：最初版本 
//////////////////////////////////////////////////////////////////////////
#ifndef _SC513_SYNUART_H_
#define _SC513_SYNUART_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "gtypes.h"

extern int fdSynRx0;
extern int fdSynRx1;

extern int fdSynTx0;
extern int fdSynTx1;
/*******************************************************************************************
*  函数功能: 初始化指定通道接收同步422总线
*  @idea 无	
*  @param chan 表示接收同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：函数调用失败；
*  @see 无
*  @note 测控组件输出速率固定为 1.2Mbps，初始化设置固定的接收速率
********************************************************************************************/
extern INT32 synuart_recv_init(UINT32 chan);


/*******************************************************************************************
*  函数功能: 初始化指定通道发送同步422总线
*  @idea 无	
*  @param chan 表示发送同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：函数调用失败；
*  @see 无
*  @note 无
********************************************************************************************/
extern INT32 synuart_send_init(UINT32 chan);


/*******************************************************************************************
*  函数功能: 读取指定通道同步422总线数据
*  @idea 无	
*  @param chan 表示接收同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @param recvbuf 表示接收数据缓存地址
*  @param len 表示需要读取的数据长度
*  @param act_len 表示实际读取的数据长度
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：调用函数失败；
*  @see 无
*  @note 无
********************************************************************************************/
extern INT32 synuart_recv(UINT32 chan, UINT8* recvbuf, UINT32 len, UINT32* act_len);


/*******************************************************************************************
*  函数功能: 发送待发送数据到指定通道的同步422总线
*  @idea 无	
*  @param chan 表示发送同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @param sendbuf 表示待发送数据地址
*  @param len 表示需要发送的数据长度
*  @param act_len 表示实际发送的数据长度
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：调用函数失败；
*  @see 无
*  @note 无
********************************************************************************************/
extern INT32 synuart_send(UINT32 chan, UINT8* sendbuf, UINT32 len, UINT32* act_len);


/*******************************************************************************************
*  函数功能: 读取指定通道接收同步422的FIFO状态
*  @idea 无	
*  @param chan 表示接收同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @param use_len 该通道FIFO已经使用的字节数
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：调用函数失败；
*  @see 无
*  @note 无
********************************************************************************************/
extern INT32 synuart_get_rx_status(UINT32 chan, UINT32* use_len);


/*******************************************************************************************
*  函数功能: 读取指定通道发送同步422的FIFO状态
*  @idea 无	
*  @param chan 表示发送同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @param use_len 该通道FIFO已经使用的字节数
*  @return ret:	
*		1) 0：函数调用成功；
*		2) -1：调用函数失败；
*  @see 无
*  @note 无
********************************************************************************************/
extern INT32 synuart_get_tx_status(UINT32 chan, UINT32* use_len);


/*******************************************************************************************
*  函数功能: 关闭指定通道的接收同步422总线
*  @idea 无	
*  @param chan 表示接收同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @return ret:	
*		1) 0：函数调用成功
*		2) -1：其他原因调用函数失败
*		3) -2：通道号无效
*  @see 无
*  @note 本函数基于标准化总线操作过程进行定义
********************************************************************************************/
extern INT32 synuart_recv_close(UINT32 chan);


/*******************************************************************************************
*  函数功能: 关闭指定通道的发送同步422总线
*  @idea 无	
*  @param chan 表示发送同步422通道序号，0代表S频段全向测控通道,1代表Ka频段全向测控通道
*  @return ret:	
*		1) 0：函数调用成功
*		2) -1：其他原因调用函数失败
*		3) -2：通道号无效
*  @see 无
*  @note 本函数基于标准化总线操作过程进行定义
********************************************************************************************/
extern INT32 synuart_send_close(UINT32 chan);


#ifdef __cplusplus
}
#endif
#endif 
