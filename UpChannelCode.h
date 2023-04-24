// UpChannelCode.h: interface for the CUpChannelCode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _UPCHANNELCODE_H
#define _UPCHANNELCODE_H

#pragma once
#define MAX_UPCHANNEL_REPORT_LEN (5*1024)
#include "ace/Process_Mutex.h"
class CUpChannelCode  
{
public:
	CUpChannelCode(char *addr,ACE_Process_Mutex *mutex);
	virtual ~CUpChannelCode();
protected:
    ACE_Process_Mutex *pMutex;
    char *pMemAddress;
public:
  /*
   *	初始化缓冲区内容
   */
  int InitMemory();
  /*
   * 获取指定长度的缓冲区内容
   * 返回：
   * buf_len 成功
   * <=0 缓冲区长度不够指定长度
  */
  int GetReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset);
  /*
   * 追加写入缓冲区指定长度的内容
   * 返回：
   * buf_len 成功
   * <=0 缓冲区长度不够
   */
  int PutReportWithLen(const char *buf,const unsigned int buf_len);
  /*
   *	清除缓冲区内容即缓冲区中的内容全部扔掉
   */
  int ClearMemory();
  /*
   *	从头部清除指定长度的缓冲区内容
   *  返回：
   *  shrink_len 成功
   *  < shrink_len && >=0 缓冲区长度不够,已全部清除
   *  <0 出错
   */
  int ShrinkMemory(const unsigned int shrink_len);
  int Show(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number,int &i_new);
private:
  int GetStartIndex();
  void SetStartIndex(int index);
  int GetEndIndex();
  void SetEndIndex(int index);
  unsigned int GetTotalNumber();
  void SetTotalNumber(unsigned int num);
  char* GetReportAddress();
  unsigned int GetCharSum();
  void SetCharSum(unsigned int sum);
};

#endif 
