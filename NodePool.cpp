// NodePool.cpp: implementation of the CNodePool class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DownReports.h"
#include "UpChannelCode.h"
#include "Report.h"
#include "NodePool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodePool::CNodePool(int node_index)
{
  m_iNodeIndex = node_index;
  sprintf(mac_shareFileName,"%ssRemoteNode%04d",POOL_DIR,node_index);
  sprintf(mac_mutexName,"mRemoteNode%04d",node_index);
  pMutex = new ACE_Process_Mutex(mac_mutexName);
}

CNodePool::~CNodePool()
{
  m_pool.close();
  delete pMutex;
  pMutex = 0;
}
int CNodePool::ResetPool(char *addr)
{
  int ih_length =  sizeof(int)+MAX_DOWN_REPORT_LEN + 5*sizeof(int)+MAX_UPCHANNEL_REPORT_LEN + sizeof(REPORT_HEAD)+MAX_REPORT_SIZE;
  ACE_OS::memset(addr,0,ih_length);
  int len = MAX_DOWN_REPORT_LEN;
  ACE_OS::memcpy(addr,&len,sizeof(int));
  len = MAX_UPCHANNEL_REPORT_LEN;
  ACE_OS::memcpy(addr+sizeof(int)+MAX_DOWN_REPORT_LEN,&len,sizeof(int));
  len = MAX_REPORT_SIZE;
  ACE_OS::memcpy(addr+sizeof(int)+MAX_DOWN_REPORT_LEN + 5*sizeof(int)+MAX_UPCHANNEL_REPORT_LEN,&len,sizeof(int));
  return 1;
}
int CNodePool::InitPool()
{
  int li_retcode = -1;
  int ih_length =  sizeof(int)+MAX_DOWN_REPORT_LEN + 5*sizeof(int)+MAX_UPCHANNEL_REPORT_LEN + sizeof(REPORT_HEAD)+MAX_REPORT_SIZE;
  pMutex->acquire();
  if (m_pool.open(mac_shareFileName,ih_length)>=0)
  {    
    char *lpv_shm=(char *)m_pool.malloc();
    if (lpv_shm!=0)
    { 
      ResetPool(lpv_shm);
      m_pool.free(lpv_shm);
      li_retcode=1;
    }
  }
  pMutex->release ();
  return li_retcode;
}
int CNodePool::OpenPool()
{
  if (m_pool.open(mac_shareFileName)>=0)
  {    
    void *lpv_mem=m_pool.malloc();
    if (lpv_mem!=0)
    {  
      m_pool.free(lpv_mem);
      return 1;
    }
  }
  return -1;
}
int CNodePool::ClosePool()
{
  m_pool.close();
  return 1;
}
int CNodePool::PutDownReport(const char *ipc_report,const unsigned int &ih_size)
{
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CDownReports downReport(lpv_mem,pMutex);
  int li_ret = downReport.PutReport(ipc_report,ih_size);
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::ClearPool()
{
	pMutex->acquire();
	char *lpv_shm=(char *)m_pool.malloc();
	if (lpv_shm!=0)
	{ 
		ResetPool(lpv_shm);
		m_pool.free(lpv_shm);
	}
	pMutex->release ();
	return 1;
}
int CNodePool::GetDownReport(char *&opc_report,unsigned int &oh_size)
{
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CDownReports downReport(lpv_mem,pMutex);
  int li_ret = downReport.GetReport(opc_report,oh_size);
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::ClearDownReport()
{
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CDownReports downReport(lpv_mem,pMutex);
  downReport.InitMemory();
  m_pool.free(lpv_mem);
  return 1;
}
int CNodePool::GetUpChannelReportWithLen(char *buf,const unsigned int buf_len,const unsigned int offset)
{
  int mem_offset = sizeof(int)+MAX_DOWN_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CUpChannelCode upChannelCode(lpv_mem+mem_offset,pMutex);
  int li_ret = upChannelCode.GetReportWithLen(buf,buf_len,offset);
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::PutUpChannelReportWithLen(const char *buf,const unsigned int buf_len)
{
  int offset = sizeof(int)+MAX_DOWN_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CUpChannelCode upChannelCode(lpv_mem+offset,pMutex);
  int li_ret = upChannelCode.PutReportWithLen(buf,buf_len);
  m_pool.free(lpv_mem);
  return li_ret;

}
int CNodePool::ClearUpChannelMemory()
{
  int offset = sizeof(int)+MAX_DOWN_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CUpChannelCode upChannelCode(lpv_mem+offset,pMutex);
  int li_ret = upChannelCode.ClearMemory();
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::ShrinkUpChannelMemory(const unsigned int shrink_len)
{
  int offset = sizeof(int)+MAX_DOWN_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CUpChannelCode upChannelCode(lpv_mem+offset,pMutex);
  int li_ret = upChannelCode.ShrinkMemory(shrink_len);
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::ShowUpChannelCode(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number,int &i_new)
{
	int offset = sizeof(int)+MAX_DOWN_REPORT_LEN;
	char *lpv_mem = (char *)m_pool.malloc();
	if(lpv_mem==0)
		return -1;
	CUpChannelCode upChannelCode(lpv_mem+offset,pMutex);
	int li_ret = upChannelCode.Show(opc_report,oh_size,bh_number,i_new);
	m_pool.free(lpv_mem);
	return li_ret;
}
int CNodePool::AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size)
{
  int offset = sizeof(int)+MAX_DOWN_REPORT_LEN + 5*sizeof(int)+MAX_UPCHANNEL_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CReport teleReport(lpv_mem+offset,pMutex);
  int li_ret = teleReport.AddTele(ih_direct,ipc_report,ih_size);
  m_pool.free(lpv_mem);
  return li_ret;
}
int CNodePool::ShowTele(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number)
{
  int offset = sizeof(int)+MAX_DOWN_REPORT_LEN + 5*sizeof(int)+MAX_UPCHANNEL_REPORT_LEN;
  char *lpv_mem = (char *)m_pool.malloc();
  if(lpv_mem==0)
    return -1;
  CReport teleReport(lpv_mem+offset,pMutex);
  int li_ret = teleReport.Show(opc_report,oh_size,bh_number);
  m_pool.free(lpv_mem);
  return li_ret;
}
