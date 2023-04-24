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
  // �����б��ķ��뻺��
	int PutDownReport(const char *ipc_report,const unsigned int &ih_size);
  // ��ȡ�����б���
	int GetDownReport(char *&opc_report,unsigned int &oh_size);
  //������б���
	int ClearDownReport();
  /*
   * ��ȡָ�����ȵĻ���������
   * ���أ�
   * buf_len �ɹ�
   * <=0 ���������Ȳ���ָ������
  */
  int GetUpChannelReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset);
  /*
   * ׷��д�뻺����ָ�����ȵ�����
   * ���أ�
   * buf_len �ɹ�
   * <=0 ���������Ȳ���
   */
  int PutUpChannelReportWithLen(const char *buf,const unsigned int buf_len);
  /*
   *	������������ݼ��������е�����ȫ���ӵ�
   */
  int ClearUpChannelMemory();
  /*
   *	��ͷ�����ָ�����ȵĻ���������
   *  ���أ�
   *  shrink_len �ɹ�
   *  < shrink_len && >=0 ���������Ȳ���,��ȫ�����
   *  <0 ����
   */
  int ShrinkUpChannelMemory(const unsigned int shrink_len);
  int ShowUpChannelCode(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number,int &i_new);
  // ���뱨����
	int AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size);
  // ��ñ�����
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
