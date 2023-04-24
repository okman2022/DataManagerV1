// UpChannelCode.cpp: implementation of the CUpChannelCode class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ace/OS.h"

#include "UpChannelCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUpChannelCode::CUpChannelCode(char *addr,ACE_Process_Mutex *mutex)
{
  pMemAddress = addr;
  pMutex = mutex;
}

CUpChannelCode::~CUpChannelCode()
{
  pMemAddress = 0;
  pMutex = 0;
}
int CUpChannelCode::GetStartIndex()
{
  int index;
  int offset = sizeof(int);
  ACE_OS::memcpy(&index,pMemAddress+offset,sizeof(int));
  return index;
}
void CUpChannelCode::SetStartIndex(int index)
{
  int offset = sizeof(int);
  ACE_OS::memcpy(pMemAddress+offset,&index,sizeof(int));
  return;
}
int CUpChannelCode::GetEndIndex()
{
  int index;
  int offset = 2*sizeof(int);
  ACE_OS::memcpy(&index,pMemAddress+offset,sizeof(int));
  return index;
}
void CUpChannelCode::SetEndIndex(int index)
{
  int offset = 2*sizeof(int);
  ACE_OS::memcpy(pMemAddress+offset,&index,sizeof(int));
  return;
}
unsigned int CUpChannelCode::GetTotalNumber()
{
	unsigned int total_num;
	int offset = 3*sizeof(int);
	ACE_OS::memcpy(&total_num,pMemAddress+offset,sizeof(unsigned int));
	return total_num;
}
void CUpChannelCode::SetTotalNumber(unsigned int num)
{
	int offset = 3*sizeof(int);
	ACE_OS::memcpy(pMemAddress+offset,&num,sizeof(unsigned int));
}
char *CUpChannelCode::GetReportAddress()
{
  return pMemAddress+5*sizeof(int);
}
unsigned int CUpChannelCode::GetCharSum()
{
    unsigned int char_sum = 0;
	int offset = 4*sizeof(int);
	ACE_OS::memcpy(&char_sum,pMemAddress+offset,sizeof(unsigned int));
	return char_sum;
}
void CUpChannelCode::SetCharSum(unsigned int sum)
{
    int offset = 4*sizeof(int);
	ACE_OS::memcpy(pMemAddress+offset,&sum,sizeof(unsigned int));
}
int CUpChannelCode::InitMemory()
{
  int len = MAX_UPCHANNEL_REPORT_LEN;
  pMutex->acquire();
  ACE_OS::memset(pMemAddress,0,5*sizeof(int)+len);
  ACE_OS::memcpy(pMemAddress,&len,sizeof(int));
  pMutex->release();
  return 1;
}
int CUpChannelCode::ClearMemory()
{
  return InitMemory();
}
/*
 * 追加写入缓冲区指定长度的内容
 * 返回：
 * buf_len 成功
 * <=0 缓冲区长度不够
*/
int CUpChannelCode::PutReportWithLen(const char *buf,const unsigned int buf_len)
{
  pMutex->acquire();
  int start_index = GetStartIndex();
  int end_index = GetEndIndex();
  int len = GetCharSum();
//  int len = end_index - start_index;
//  if(len > 0)
//    len +=1;
//  else if(len <0)
//    len = len + MAX_UPCHANNEL_REPORT_LEN +1;
  unsigned int free_len = MAX_UPCHANNEL_REPORT_LEN-len;
  if(free_len < buf_len)
  {
    pMutex->release();
    return -100;
  }
  bool bOver = false;
  if(end_index + buf_len >= MAX_UPCHANNEL_REPORT_LEN)
    bOver = true;
  int start_offset=end_index+1;
  if(len==0)
	  SetStartIndex(start_offset);
  if(bOver)
  {
    int temp_len = MAX_UPCHANNEL_REPORT_LEN-start_offset;
    ACE_OS::memcpy(GetReportAddress()+start_offset,buf,temp_len);
    ACE_OS::memcpy(GetReportAddress(),buf+temp_len,buf_len-temp_len);
  }
  else
    ACE_OS::memcpy(GetReportAddress()+start_offset,buf,buf_len);
  int tempint = (start_offset+buf_len-1)%MAX_UPCHANNEL_REPORT_LEN;
  SetEndIndex(tempint);
  SetCharSum(GetCharSum()+buf_len);
  SetTotalNumber(GetTotalNumber()+buf_len);
  pMutex->release();
  return buf_len;
}
/*
 * 获取指定长度的缓冲区内容
 * 返回：
 * buf_len 成功
 * <=0 缓冲区长度不够指定长度
*/
int CUpChannelCode::GetReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset)
{
  pMutex->acquire();
  unsigned int ltm = GetTotalNumber();
  int start_index = GetStartIndex();
  int end_index = GetEndIndex();
  int len = GetCharSum();
//  int len = end_index - start_index;
//  if(len > 0)
//    len +=1;
//  else if(len <0)
//    len = len + MAX_UPCHANNEL_REPORT_LEN +1;
  if((unsigned int)len < buf_len+offset)
  {
    pMutex->release();
    return -1;
  }
  bool bOver = false;
  int temp_start_index = (start_index+offset)%MAX_UPCHANNEL_REPORT_LEN;
  if(temp_start_index+buf_len > MAX_UPCHANNEL_REPORT_LEN)
    bOver = true;
  if(bOver)
  {
	int temp_len = MAX_UPCHANNEL_REPORT_LEN-temp_start_index;
    ACE_OS::memcpy(buf,GetReportAddress()+temp_start_index,temp_len);
    ACE_OS::memcpy(buf+temp_len,GetReportAddress(),buf_len-temp_len);
  }
  else
     ACE_OS::memcpy(buf,GetReportAddress()+temp_start_index,buf_len);
  pMutex->release();
  return buf_len;
}
/*
 *	从头部清除指定长度的缓冲区内容
 *  返回：
 *  shrink_len 成功
 *  < shrink_len && >=0 缓冲区长度不够,已全部清除
 *  <0 出错
*/
int CUpChannelCode::ShrinkMemory(const unsigned int shrink_len)
{
  int real_shrink_len;
  pMutex->acquire();
  int start_index = GetStartIndex();
  int end_index = GetEndIndex();
  int len = GetCharSum();
//  int len = end_index - start_index;
//  if(len > 0)
//    len +=1;
//  else if(len <0)
//    len = len + MAX_UPCHANNEL_REPORT_LEN +1;
  if(len ==0)
    real_shrink_len =0;
  if((unsigned int)len > shrink_len)
  {
    SetStartIndex((GetStartIndex()+shrink_len)%MAX_UPCHANNEL_REPORT_LEN);
    SetCharSum(len-shrink_len);
    real_shrink_len = shrink_len;
  }
  else
  {
    SetStartIndex(GetEndIndex());
    SetCharSum(0);
    real_shrink_len = len;
  }
  pMutex->release();
  return real_shrink_len;
}

// 获得报文
int CUpChannelCode::Show(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number,int &i_new)
{
	char *lpc_shm=0;
	int li_retcode=-1;
	oh_size=0;
	pMutex->acquire();
	int start_index = GetStartIndex();
	int end_index = GetEndIndex();
	int len = end_index - start_index;
	if(len > 0)
		len +=1;
	else if(len <0)
		len = len + MAX_UPCHANNEL_REPORT_LEN +1;
	unsigned int total_number = GetTotalNumber();
	if ((total_number-bh_number) > MAX_UPCHANNEL_REPORT_LEN ||bh_number==0 ||total_number<bh_number)
	{
		int start_offset;
		if(end_index < total_number)
		{
			oh_size = end_index+1;
			start_offset = 0;
		}
		else
		{
			oh_size = total_number;
			start_offset = 1;
		}
		i_new = 1;
		bh_number = total_number;
		opc_report = (char *)ACE_OS::malloc(oh_size);
		ACE_OS::memcpy(opc_report,GetReportAddress()+start_offset,oh_size);
	}
	else
	{
		oh_size = total_number-bh_number;
		i_new = 0;
		bh_number = total_number;
		if(oh_size==0)
		{
			pMutex->release();
			return 0;
		}
		int temp_start_index;
		bool bOver = false;
		if(end_index >= oh_size)
			temp_start_index = end_index - oh_size+1;
		else
		{
			bOver = true;
			temp_start_index = MAX_UPCHANNEL_REPORT_LEN - oh_size + end_index +1;
		}
		opc_report = (char *)ACE_OS::malloc(oh_size);
		if(!bOver)
			ACE_OS::memcpy(opc_report,GetReportAddress()+temp_start_index,oh_size);
		else
		{
			int temp_len = MAX_UPCHANNEL_REPORT_LEN - temp_start_index;
			ACE_OS::memcpy(opc_report,GetReportAddress()+temp_start_index,temp_len);
			ACE_OS::memcpy(opc_report+temp_len,GetReportAddress(),oh_size-temp_len);
		}
	}
	//TRACE("is_new=%d,oh_size=%d,bh_number=%d,total_number=%d\n",i_new,oh_size,bh_number,total_number);
	pMutex->release();
	return oh_size;
}
