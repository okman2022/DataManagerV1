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
   *	��ʼ������������
   */
  int InitMemory();
  /*
   * ��ȡָ�����ȵĻ���������
   * ���أ�
   * buf_len �ɹ�
   * <=0 ���������Ȳ���ָ������
  */
  int GetReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset);
  /*
   * ׷��д�뻺����ָ�����ȵ�����
   * ���أ�
   * buf_len �ɹ�
   * <=0 ���������Ȳ���
   */
  int PutReportWithLen(const char *buf,const unsigned int buf_len);
  /*
   *	������������ݼ��������е�����ȫ���ӵ�
   */
  int ClearMemory();
  /*
   *	��ͷ�����ָ�����ȵĻ���������
   *  ���أ�
   *  shrink_len �ɹ�
   *  < shrink_len && >=0 ���������Ȳ���,��ȫ�����
   *  <0 ����
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
