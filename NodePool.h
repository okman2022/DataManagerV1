// NodePool.h: interface for the CNodePool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NODEPOOL_H
#define _NODEPOOL_H

#pragma once

#ifdef WIN32
#ifndef DLL_API
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#endif
#else
#define DLL_API
#endif

#include "ace/Shared_Memory_MM.h"
#include "ace/Process_Mutex.h"
#define POOL_DIR "./tmp/"

class  DLL_API CNodePool  
{
public:
	CNodePool(int node_index);
	virtual ~CNodePool();
  int InitPool();
  int OpenPool();
  int ClosePool();
  int ClearPool();
public:
  // 将下行报文放入缓冲
	int PutDownReport(const char *ipc_report,const unsigned int &ih_size);
  // 将取出下行报文
	int GetDownReport(char *&opc_report,unsigned int &oh_size);
  //清除下行报文
	int ClearDownReport();
  /*
   * 获取指定长度的缓冲区内容
   * 返回：
   * buf_len 成功
   * <=0 缓冲区长度不够指定长度
  */
  int GetUpChannelReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset);
  /*
   * 追加写入缓冲区指定长度的内容
   * 返回：
   * buf_len 成功
   * <=0 缓冲区长度不够
   */
  int PutUpChannelReportWithLen(const char *buf,const unsigned int buf_len);
  /*
   *	清除缓冲区内容即缓冲区中的内容全部扔掉
   */
  int ClearUpChannelMemory();
  /*
   *	从头部清除指定长度的缓冲区内容
   *  返回：
   *  shrink_len 成功
   *  < shrink_len && >=0 缓冲区长度不够,已全部清除
   *  <0 出错
   */
  int ShrinkUpChannelMemory(const unsigned int shrink_len);
  int ShowUpChannelCode(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number,int &i_new);
  // 加入报文码
	int AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size);
  // 获得报文码
	int ShowTele(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number);
private:
  int ResetPool(char *addr);
private:
  int m_iNodeIndex;
  ACE_Shared_Memory_MM m_pool;
  ACE_Process_Mutex *pMutex;
  char mac_shareFileName[40];
  char mac_mutexName[20];
};

#endif 
