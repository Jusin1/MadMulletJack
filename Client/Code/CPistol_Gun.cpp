#include "pch.h"
#include "CPistol_Gun.h"
#include "CTimerMgr.h"
#include "CManagement.h"
#include "CObjectManager.h"

CPistol_Gun::CPistol_Gun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGun(pGraphicDev)
{
}

CPistol_Gun::CPistol_Gun(const CPistol_Gun& rhs)
	:CGun(rhs)
{
}

CPistol_Gun::~CPistol_Gun()
{
}

HRESULT CPistol_Gun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_WapPisotl"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPistol_Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bIsInfinite = false;

	// 파워 / 정확도 / 속도
	m_iPower = 3;
	m_iPrecision = 10;
	m_iSpeed = 5;
	// 최대 불렛
	m_iMaxBullet = 3; //origin : 9 / debug 3
	m_iBullet = m_iMaxBullet;

	Set_Texture();

	return S_OK;
}

_int CPistol_Gun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	// 만약 지금 idle texture 라면
	if (m_CurrentAnimTag == TEXT("Com_Texture_Pistol_Idle"))
	{
		SCENE eCurScene = (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx();
		Engine::CTransform* pHandRformCom = nullptr;

		// handR의 위치 받아옴
		switch (eCurScene)
		{
		case SCENE_DEV:
			pHandRformCom =
				dynamic_cast<CTransform*>(CObjectManager::GetInstance()->
					Get_Component(SCENE_DEV, L"UI_Layer", L"Com_Transform", 2));
			break;

		case SCENE_TUTORIAL:
			pHandRformCom =
				dynamic_cast<CTransform*>(CObjectManager::GetInstance()->
					Get_Component(SCENE_TUTORIAL, L"UI_Layer", L"Com_Transform", 6));
			break;
		}
		
		if (pHandRformCom)
		{
			// 위치를 통해 pos update
			Set_UIPos(pHandRformCom->Get_Info(INFO_POS), -120.f, 350.f);
		}
	}

	// 만약 지금 idle texture가 아니고 ani가 끝났다면
	else if (m_pTextureCom->Is_AnimFinished())
	{
		// state 끝났다고 알려줌
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
	}

	return NO_EVENT;
}

void CPistol_Gun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

}

void CPistol_Gun::Render_GameObject()
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

	__super::Render_GameObject();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CPistol_Gun::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Idle", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Idle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Idle"), m_pTextureCom });

	// Opening
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 9;
	texInfo.m_fSpeed = 25.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Op", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Op", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Op"), m_pTextureCom });

	// Attack
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 6;
	texInfo.m_fSpeed = 20.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Att", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Attack", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Att"), m_pTextureCom });

	// Reload
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 13;
	texInfo.m_fSpeed = 20.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Re", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Re", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Re"), m_pTextureCom });

	// CIdle
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_C_Idle", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Idle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_C_Idle"), m_pTextureCom });

	// CAttack
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 5;
	texInfo.m_fSpeed = 0.5f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_C_Att", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Attack", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_C_Att"), m_pTextureCom });

	// CZooming
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_C_Zooming", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Zooming", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_C_Zooming"), m_pTextureCom });

	// CZoomIdle
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_C_ZoomIdle", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomIdle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_C_ZoomIdle"), m_pTextureCom });

	// CZoomAtt
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_C_ZoomAtt", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomAtt", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_C_ZoomAtt"), m_pTextureCom });

	return S_OK;
}

HRESULT CPistol_Gun::Set_Texture() {
	//IDLE, JUMP, KICK, ATTACK,
	//ATTACK_INSTANT, ZOOMING, ZOOM, RELOAD, DOPING, OPENING, PLAYERDEAD, CLEAR, PLAYER_END
	m_bRenderOn = true;

	SCENE eScene = (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx();

	// scene car 에서 쓰는 texture가 아예 달라서 scene 별로 나누어서 결정
	if (eScene == SCENE_CAR)
	{
		switch (m_tInfo.ePlayerState)
		{
		case IDLE:
			break;
		case ATTACK:
			break;
		case ZOOMING:
			break;
		}
	}

	else 
	{
		switch (m_tInfo.ePlayerState)
		{
		case OPENING:
			if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Op"))))
				return E_FAIL;
			Set_UISizeAndPos(201.f, 457.f, WINCX * 0.5f + 350.f, WINCY * 0.5f );

			Set_New_TransInfo(500.f, 0.f);
			
			break;

		case ATTACK:
			if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Att"))))
				return E_FAIL;
			Set_UISizeAndPos(245.f, 500.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 200.f);

			m_iBullet--;

			break;

		case ATTACK_INSTANT:
			m_bRenderOn = false;
			break;

		case RELOAD:
			if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Re"))))
				return E_FAIL;
			Set_UISizeAndPos(360.f, 660.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 150.f);

			Reload_Bullet();

			break;

		case PLAYERDEAD:
			m_bActive = false;
			break;

		default:
			if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Idle"))))
				return E_FAIL;
			Set_UISizeAndPos(165.f, 500.f, WINCX * 0.5f + 300.f, WINCY * 0.5f + 200.f); // pos를 정하고
			//pPistol->Set_UIPos(m_pTransformCom->Get_Info(INFO_POS), -120.f, 350.f);

			break;
		}
	}

	return S_OK;
}

HRESULT CPistol_Gun::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

CPistol_Gun* CPistol_Gun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPistol_Gun* pInstance = new CPistol_Gun(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPistol_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPistol_Gun::Clone(void* pArg)
{
	CPistol_Gun* pInstance = new CPistol_Gun(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPistol_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPistol_Gun::Free()
{
	__super::Free();
}
