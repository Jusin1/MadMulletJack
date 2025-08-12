#include "pch.h"
#include "CGlobal_Info.h"

CGlobal_Info* CGlobal_Info::m_pInstance = nullptr;

CGlobal_Info::CGlobal_Info() :m_tPlayerInfo({ PLAYER_END, WP_END, WP2_END })
{
}

CGlobal_Info::~CGlobal_Info()
{
}

void CGlobal_Info::Initialize()
{
	m_tPlayerInfo.ePlayerState = IDLE;
	m_tPlayerInfo.eWeapon = WP_NON;
	m_tPlayerInfo.eWeapon2 = WP_KICK;
}