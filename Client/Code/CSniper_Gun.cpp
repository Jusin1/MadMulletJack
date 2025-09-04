#include "pch.h"
#include "CSniper_Gun.h"
#include "Sound_Manager.h"

CSniper_Gun::CSniper_Gun(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGun(pGraphicDev)
{
}

CSniper_Gun::CSniper_Gun(const CSniper_Gun& rhs)
	: CGun(rhs)
{
}

CSniper_Gun::~CSniper_Gun()
{
}

HRESULT CSniper_Gun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSniper_Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bIsInfinite = true;

	m_fCoolTime = 0.1f;

	Set_Texture();

	return S_OK;
}

_int CSniper_Gun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	Move_UI(fTimeDelta);

	return NO_EVENT;
}

void CSniper_Gun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 지금 idle texture가 아니고 ani가 끝났다면
	if (m_CurrentAnimTag != TEXT("Com_Texture_Sniper_Idle") &&
		m_pTextureCom->Is_AnimFinished())
	{
		// state 끝났다고 알려줌
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
	}
}

void CSniper_Gun::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	m_pTextureCom->MoveFrame();
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CUI::Render_GameObject();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CSniper_Gun::Set_Texture()
{
	m_bRenderOn = true;
	
	m_tMoveInfo.eUIMove = MV_NON;

	switch (m_tInfo.ePlayerState)
	{
	case ATTACK:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Attack"))))
			return E_FAIL;
		Set_UISizeAndPos(590.f, 810.f, WINCX * 0.5f + 400.f, WINCY * 0.5f + 380.f);
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/weapon/sniper/sfx_gp_wp_sniper_air_01.wav", SOUND_WEAPON, 2.f, false);
		break;
		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\sniper\sfx_gp_wp_sniper_air_01.wav"
		

	case ZOOMING:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Zooming"))))
			return E_FAIL;
		Set_UISizeAndPos(1014.f, 903.f, WINCX * 0.5f + 350.f, WINCY * 0.5f);

		break;

	case ZOOM:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Zoom"))))
			return E_FAIL;
		Set_UISizeAndPos(1420.f, 1390.f, WINCX * 0.5f - 15.4f, WINCY * 0.5f - 102.f );


		break;

	case OPENING:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Op"))))
			return E_FAIL;
		Set_UISizeAndPos(1014.f, 903.f, WINCX * 0.5f, WINCY * 0.5f + 100.f);

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\sniper\sfx_wp_sniper_intro.wav"
		CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/weapon/sniper/sfx_wp_sniper_intro.wav", SOUND_WEAPON, 2.f, 2.5f,false);

		break;

	case PLAYERDEAD:
		m_bActive = false;
		break;

	case ATTEND:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_AttackEnd"))))
			return E_FAIL;
		Set_UISizeAndPos(1014.f, 903.f, WINCX * 0.5f, WINCY * 0.5f + 100.f);

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\sniper\sfx_wp_sniper_reload.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/weapon/sniper/sfx_wp_sniper_reload.wav", SOUND_WEAPON, 2.f, false);

		break;

	case ATTACK_ZOOM:
		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\sniper\sfx_gp_wp_sniper_shot_06.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/weapon/sniper/sfx_gp_wp_sniper_shot_06.wav", SOUND_WEAPON, 2.f, false);
		break;

	case ZOOMOUT:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_ZoomOut"))))
			return E_FAIL;
		Set_UISizeAndPos(1014.f, 903.f, WINCX * 0.5f + 350.f, WINCY * 0.5f);

		break;


	default:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Idle"))))
			return E_FAIL;
		Set_UISizeAndPos(590.f, 810.f, WINCX * 0.5f + 400.f, WINCY * 0.5f + 380.f);
		Set_New_TransInfo(5.f,0.f);
		m_tMoveInfo = {MV_UpDown , false, 5.f,0.f};

		break;
	}

	return S_OK;
}

HRESULT CSniper_Gun::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Idle", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Idle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Idle"), m_pTextureCom });

	// OP
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 30;
	texInfo.m_fSpeed = 20.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Op", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Op", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Op"), m_pTextureCom });

	// attack
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 2;
	texInfo.m_fSpeed = 10.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Attack", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Attack", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Attack"), m_pTextureCom });

	// attack end
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 14;
	texInfo.m_fSpeed = 10.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_AttackEnd", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_AttEnd", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_AttackEnd"), m_pTextureCom });

	// zoooming
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 6;
	texInfo.m_fSpeed = 10.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Zooming", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Zooming", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Zooming"), m_pTextureCom });

	// zooom
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 1;
	texInfo.m_fSpeed = 0.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Zoom", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Zoom", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Zoom"), m_pTextureCom });

	// attack zoom
	// zoom texture 유지

	// zoomout
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 7;
	texInfo.m_fSpeed = 10.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_ZoomOut", SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_ZoomAtt", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_ZoomOut"), m_pTextureCom });

	return S_OK;
}

HRESULT CSniper_Gun::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

CSniper_Gun* CSniper_Gun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSniper_Gun* pInstance = new CSniper_Gun(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSniper_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSniper_Gun::Clone(void* pArg)
{
	CSniper_Gun* pInstance = new CSniper_Gun(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSniper_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSniper_Gun::Free()
{
	__super::Free();
}
