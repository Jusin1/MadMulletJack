#include "pch.h"
#include "CPlayer_StateInfo.h"

CPlayer_StateInfo* CPlayer_StateInfo::m_pInstance = nullptr;

CPlayer_StateInfo::CPlayer_StateInfo() :m_tPlayerInfo({ PLAYER_END, WP_END, WP2_END })
{
}

CPlayer_StateInfo::~CPlayer_StateInfo()
{
}

void CPlayer_StateInfo::Initialize()
{
	m_tPlayerInfo.ePlayerState	= IDLE;
	m_tPlayerInfo.eWeapon		= WP_NON;
	m_tPlayerInfo.eWeapon2		= WP_KICK;

}