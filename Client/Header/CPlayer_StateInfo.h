#pragma once
#include "Clinet_Define.h"

class CPlayer_StateInfo
{
public:
	CPlayer_StateInfo();
	CPlayer_StateInfo(const CPlayer_StateInfo&) = delete;
	CPlayer_StateInfo& operator=(const CPlayer_StateInfo&) = delete;
	~CPlayer_StateInfo();

public:
	void Initialize(); // 값 초기화가 필요할 때

	const PlayerStateInfo Get_PlayerInfo() const { return m_tPlayerInfo; }
	void Set_PlayerInfo( PlayerStateInfo _tInfo) { m_tPlayerInfo = _tInfo; }

public:
	static CPlayer_StateInfo* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CPlayer_StateInfo;
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
	static CPlayer_StateInfo* m_pInstance;

private:
	PlayerStateInfo m_tPlayerInfo;
};