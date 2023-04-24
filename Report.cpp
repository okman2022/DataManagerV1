#include "StdAfx.h"
#include "ace/OS.h"
#include "Report.h"
//#include "Mnet/define.h"

CReport::CReport(char *addr,ACE_Process_Mutex *mutex)
{
	pMemAddress = addr;
	pMutex = mutex;
}

CReport::~CReport(void)
{
	pMemAddress = 0;
	pMutex = 0;
}
void CReport::GetReportHead(REPORT_HEAD &ReportHead)
{
	ACE_OS::memcpy((char *)&ReportHead,pMemAddress,sizeof(REPORT_HEAD));
	return;
}
void CReport::SetReportHead(const REPORT_HEAD ReportHead)
{
	ACE_OS::memcpy(pMemAddress,(char *)&ReportHead,sizeof(REPORT_HEAD));
	return;
}
char* CReport::GetTeleAddress()
{
	return pMemAddress + sizeof(REPORT_HEAD);
}
// 初始化
int CReport::InitMemory(void)
{
	int len = MAX_REPORT_SIZE;
	pMutex->acquire();
	ACE_OS::memset(pMemAddress,0,5*sizeof(int)+MAX_REPORT_SIZE);
	ACE_OS::memcpy(pMemAddress,&len,sizeof(int));
	pMutex->release();
	return 1;
}

// 加入报文
int CReport::AddTele(const unsigned int ih_direct,const char *ipc_report,const unsigned int ih_size)
{
	char *lpc_shm=0;
	if(ih_size==0 || ih_size>MAX_REPORT_SIZE)
		return -1;
	pMutex->acquire();
	REPORT_HEAD lReportHead;
	GetReportHead(lReportHead);
	//准备报文缓冲
	TELE_HEAD l_hc;
	l_hc.direct=ih_direct;
	l_hc.realLen=ih_size;
	if ((ih_size%sizeof(l_hc))!=0)
		l_hc.len=ih_size+sizeof(l_hc)+(sizeof(l_hc)-ih_size%sizeof(l_hc));
	else
		l_hc.len= ih_size+sizeof(l_hc);
	//判断空间是否已满
	if ((lReportHead.total_len + l_hc.len)>MAX_REPORT_SIZE)
		RemoveTele(&lReportHead,l_hc.len);
	//得到当前缓冲区末指针
	unsigned int lh_offset=lReportHead.cur_pos + lReportHead.total_len;
	lh_offset %=MAX_REPORT_SIZE;
	char *lpc_temp=GetTeleAddress()+lh_offset;
	if ( (l_hc.len + lh_offset) > MAX_REPORT_SIZE )
	{
		unsigned int lh_tempLen = MAX_REPORT_SIZE -lh_offset;
		ACE_OS::memcpy(lpc_temp,&l_hc,sizeof(l_hc));
		ACE_OS::memcpy(lpc_temp+sizeof(l_hc),ipc_report,lh_tempLen-sizeof(l_hc));
		lpc_temp=GetTeleAddress();
		ACE_OS::memcpy(lpc_temp,ipc_report+lh_tempLen-sizeof(l_hc),ih_size-lh_tempLen+sizeof(l_hc));
	}
	else
	{
		ACE_OS::memcpy(lpc_temp,&l_hc,sizeof(l_hc));
		ACE_OS::memcpy(lpc_temp+sizeof(l_hc),ipc_report,ih_size);
	}
	lReportHead.number ++;
	lReportHead.total_len += l_hc.len;
	lReportHead.total_number ++;
	SetReportHead(lReportHead);
	pMutex->release();
	return (int) ih_size;
}

// 从缓冲中移去报文
int CReport::RemoveTele(REPORT_HEAD* ip_Head,const unsigned int ih_size)
{
	while (MAX_REPORT_SIZE-ip_Head->total_len<ih_size )
	{
		TELE_HEAD *lp_HeadContent = (TELE_HEAD *)(GetTeleAddress()+ip_Head->cur_pos);
		ip_Head->cur_pos = ip_Head->cur_pos + lp_HeadContent->len;
		ip_Head->cur_pos %=MAX_REPORT_SIZE;
		ip_Head->total_len -=lp_HeadContent->len;
		ip_Head->number --;
	}
	return 0;
}

// 获得报文
int CReport::Show(char *&opc_report,unsigned int &oh_size,unsigned int &bh_number)
{
	char *lpc_shm=0;
	int li_retcode=-1;
	oh_size=0;
	pMutex->acquire();
	REPORT_HEAD lReportHead;
	GetReportHead(lReportHead);
	if ((lReportHead.total_number-bh_number) > lReportHead.number ||
		bh_number==0 || lReportHead.total_number==0 ||lReportHead.total_number<bh_number)
	{
		int li_counter=lReportHead.number;
		unsigned int lh_offset=lReportHead.cur_pos ;
		TELE_HEAD *lp_HeadContent=0;
		unsigned int li_reportLen=0;
		for (int i=0;i<li_counter;++i)
		{
			lh_offset %=MAX_REPORT_SIZE;
			lp_HeadContent=(TELE_HEAD*)(GetTeleAddress()+lh_offset);
			li_reportLen = lp_HeadContent->len;
			opc_report=(char *)ACE_OS::realloc(opc_report,oh_size+li_reportLen);
			if ( (lh_offset + li_reportLen ) > MAX_REPORT_SIZE)
			{
				unsigned int lh_tempLen=MAX_REPORT_SIZE - lh_offset;
				ACE_OS::memcpy( opc_report+oh_size,(unsigned char *)lp_HeadContent,lh_tempLen);
				ACE_OS::memcpy( opc_report+oh_size+lh_tempLen,GetTeleAddress(),li_reportLen-lh_tempLen);
			}
			else
				ACE_OS::memcpy(opc_report+oh_size,(unsigned char *)lp_HeadContent,li_reportLen);
			lh_offset +=li_reportLen;
			oh_size +=li_reportLen;
		}
		bh_number=lReportHead.total_number;
		li_retcode=lReportHead.number ;
	}
	else
	{
		int li_counter=lReportHead.number - (lReportHead.total_number-bh_number);
		unsigned int lh_offset=lReportHead.cur_pos ;
		TELE_HEAD *lp_HeadContent=0;
		for (int j=0;j<li_counter;++j)
		{
			lh_offset %=MAX_REPORT_SIZE;
			lp_HeadContent=(TELE_HEAD*)(GetTeleAddress()+lh_offset);
			lh_offset +=lp_HeadContent->len;
		}
		li_counter=lReportHead.total_number-bh_number;
		unsigned int li_reportLen=0;
		for (int i=0;i<li_counter;++i)
		{
			lh_offset %=MAX_REPORT_SIZE;
			lp_HeadContent=(TELE_HEAD*)(GetTeleAddress()+lh_offset);
			li_reportLen = lp_HeadContent->len;
			opc_report=(char *)ACE_OS::realloc(opc_report,oh_size+li_reportLen);
			if ( (lh_offset + li_reportLen ) > MAX_REPORT_SIZE)
			{
				unsigned int lh_tempLen=MAX_REPORT_SIZE - lh_offset;
				ACE_OS::memcpy( opc_report+oh_size,(unsigned char *)lp_HeadContent,lh_tempLen);
				ACE_OS::memcpy( opc_report+oh_size+lh_tempLen,GetTeleAddress(),li_reportLen-lh_tempLen);
			}
			else
				ACE_OS::memcpy(opc_report+oh_size,(unsigned char *)lp_HeadContent,li_reportLen);
			lh_offset +=li_reportLen;
			oh_size +=li_reportLen;
		}
		bh_number=lReportHead.total_number;
		li_retcode=li_counter ;
	}
	pMutex->release();
	return li_retcode;
}
