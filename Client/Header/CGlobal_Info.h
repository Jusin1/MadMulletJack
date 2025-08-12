#pragma once
#include "Clinet_Define.h"

// CPlayer -> playerInfo update
// 다른 객체들이 STATE update

class CGlobal_Info
{
public:
	CGlobal_Info();
	CGlobal_Info(const CGlobal_Info&) = delete;
	CGlobal_Info& operator=(const CGlobal_Info&) = delete;
	~CGlobal_Info();

public:
	void Initialize(); // 값 초기화가 필요할 때

	const PlayerStateInfo Get_PlayerInfo() const { return m_tPlayerInfo; }
	void Set_PlayerInfo( PlayerStateInfo _tInfo) { m_tPlayerInfo = _tInfo; }

	const STATE Get_STATE() const { return m_eSTATE; }
	void Set_STATE(STATE _eState) { m_eSTATE = _eState; }

	bool IS_STATE_END() { return (m_eSTATE == STATE_END); } // state가 끝났는지 bool 값으로 전해줌

public:
	static CGlobal_Info* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CGlobal_Info;
		}

		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CGlobal_Info* m_pInstance;

private:
	PlayerStateInfo m_tPlayerInfo;	// player가 지금 무슨 state들을 가지고 있는지
	STATE			m_eSTATE;		// 현재의 state의 진행 상황 : 시작, 진행, 끝
};