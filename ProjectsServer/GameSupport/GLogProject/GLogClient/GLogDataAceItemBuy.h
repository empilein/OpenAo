//////////////////////////////////
// 2011-12-12 by hskim, GLog 2��

#pragma once
#include "glogdatabase.h"

class CGLogDataAceItemBuy : public CGLogDataBase
{
public:
	CGLogDataAceItemBuy(_RecordsetPtr i_pSourceRecordSet,_CommandPtr i_pSourceCommand,_RecordsetPtr i_pDestRecordSet,_CommandPtr i_pDestCommand);
	~CGLogDataAceItemBuy(void);

	virtual Err_t	GLogProcessing();

private:
	virtual Err_t	GLogDBSelect(int * o_pSelectCnt);
	virtual Err_t	GLogDBInsert(int i_nCnt);
	virtual Err_t	GLogDBDelete(int i_nCnt);

	virtual	void	ResetData();
	virtual void	DBInsertParametersAppend(int i_nCnt);					// OleDB Paramter Append
	virtual void	DBInsertParametersDelete();								// OleDB Paramter Delete

	CAceItemBuyData	m_arrAceItemBuyData[GLOG_DATA_MAX_CNT];
};
