#pragma once


// 레퍼런스 카운트를 관리 (증가 or 감소)
class _declspec(dllexport) CBase
{
protected:
	inline explicit CBase();
	inline virtual ~CBase();

public:
	inline unsigned long Add_Ref(); // 증가
	inline unsigned long Release(); // 감소

protected:
	unsigned long m_dwRefCnt;

public:
	inline virtual void		Free() = 0;
};

#include "CBase.inl"