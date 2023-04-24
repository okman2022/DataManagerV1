#ifndef __REPORT_H
#define __REPORT_H
#pragma once
#include "ace/Process_Mutex.h"
#define MAX_REPORT_SIZE (12*1024) //����Ϊsizeof(TELE_HEAD)�ı���

struct _report_head
{
  unsigned int buffer_len;
  unsigned int number;//�������б��ĸ���
	unsigned int total_len;//��Ч�����ܳ���		
	unsigned int cur_pos;//���ݿ�ʼλ��
	unsigned int total_number;//�����ܸ���
};
typedef struct _report_head REPORT_HEAD;
struct _tele_head
{
  unsigned int direct;
	unsigned int len;//Ϊ����
  unsigned int realLen;
};
typedef struct _tele_head TELE_HEAD;

class CReport
{
public:
	CReport(char *addr,ACE_Process_Mutex *mutex);
	~CReport(void);
protected:
	char *pMemAddress;
  ACE_Process_Mutex *pMutex;
public:
	// ��ʼ��
	int InitMemory(void);
	// ���뱨��
	int AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size);
  // ��ñ���
	int Show(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number);
private:
	// �ӻ�������ȥ����
	int RemoveTele(REPORT_HEAD *ip_Head,const unsigned int ih_size);
  void GetReportHead(REPORT_HEAD &ReportHead);
  void SetReportHead(const REPORT_HEAD ReportHead);
  char* GetTeleAddress();
public:
	
};
#endif 
