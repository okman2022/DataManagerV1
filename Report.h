#ifndef __REPORT_H
#define __REPORT_H
#pragma once
#include "ace/Process_Mutex.h"
#define MAX_REPORT_SIZE (12*1024) //必须为sizeof(TELE_HEAD)的倍数

struct _report_head
{
  unsigned int buffer_len;
  unsigned int number;//缓冲区中报文个数
	unsigned int total_len;//有效数据总长度		
	unsigned int cur_pos;//数据开始位置
	unsigned int total_number;//报文总个数
};
typedef struct _report_head REPORT_HEAD;
struct _tele_head
{
  unsigned int direct;
	unsigned int len;//为对齐
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
	// 初始化
	int InitMemory(void);
	// 加入报文
	int AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size);
  // 获得报文
	int Show(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number);
private:
	// 从缓冲中移去报文
	int RemoveTele(REPORT_HEAD *ip_Head,const unsigned int ih_size);
  void GetReportHead(REPORT_HEAD &ReportHead);
  void SetReportHead(const REPORT_HEAD ReportHead);
  char* GetTeleAddress();
public:
	
};
#endif 
