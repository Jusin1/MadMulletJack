#pragma once
#include "Engine_Define.h"
#include "CBase.h"
#include "CUIBase.h"


// UI 생성 및 관리
class CUIManager :
    public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	explicit CUIManager();
	virtual ~CUIManager();
	
public:
	void CreateEnterUI();


public:
	virtual void Free() override;


private:
	CUIBase*					m_pEnterUI; // 게임 시작시 등장하는 화면
	CUIBase*					m_pMonsterDieEffect; // 몬스터 사망 시 등장하는 이펙트 UI
};

