#pragma once
#include "Clinet_Define.h"

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
	PlayerStateInfo m_tPlayerInfo;
};