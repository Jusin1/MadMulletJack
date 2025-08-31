#include "pch.h"
#include "CShot_Gun.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CImageUI.h"
#include "Engine_Function.h"

CShot_Gun::CShot_Gun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGun(pGraphicDev)
{
}

CShot_Gun::CShot_Gun(const CShot_Gun& rhs)
	:CGun(rhs)
{
}

CShot_Gun::~CShot_Gun()
{
}

HRESULT CShot_Gun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShot_Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bIsInfinite = false;

	// 파워 / 정확도 / attack cool time
	m_iPower = 10;
	m_iPrecision = 3;
	m_fCoolTime = 0.3f;
	// 최대 불렛
	m_iMaxBullet = 6; //origin : 6 / test 3
	m_iBullet = m_iMaxBullet;

	Set_Texture();

	return S_OK;
}

_int CShot_Gun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	// 만약 지금 idle texture 라면
	if (m_CurrentAnimTag == TEXT("Com_Texture_ShotGun_Idle"))
	{
		// scene을 받아옴
		_uint iCurScene = CMapFactory::GetInstance()->GetTargetSceneIndex();

		// handR을 받아옴
		CUIBase* pHandR = dynamic_cast<CUIBase*>(CObjectManager::GetInstance()
			->Find_Object(iCurScene, L"UI_Layer", 1))
			->Find_Child_ByTag(L"HandRUI");

		// handR 위치를 기준으로 pos 갱신 (offset 적용)
		if (pHandR)
		{
			Set_UIPos(pHandR->GetTransform()->Get_Info(INFO_POS), -120.f, 350.f);
		}
	}

	// 만약 지금 idle texture가 아니고 ani가 끝났다면
	else if (m_pTextureCom->Is_AnimFinished())
	{
		// state 끝났다고 알려줌
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
		DeleteEff();
	}

	return NO_EVENT;
}

void CShot_Gun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CShot_Gun::Render_GameObject()
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

HRESULT CShot_Gun::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	return S_OK;
}

HRESULT CShot_Gun::Set_Texture() {
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
			Set_UISizeAndPos(201.f, 457.f, WINCX * 0.5f + 350.f, WINCY * 0.5f - 50.f);

			break;

		case ATTACK:
			if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Att"))))
				return E_FAIL;
			Set_UISizeAndPos(360.f, 720.f, WINCX * 0.5f + 460.f, WINCY * 0.5f + 200.f);

			m_iBullet--;

			SpawnEff();

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

HRESULT CShot_Gun::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

void CShot_Gun::SpawnEff()
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;

	const _vec3 base = m_pTransformCom->Get_Info(INFO_POS);

	float fxW = 510.f, fxH = 300.f, offX = -350.f, offY = 160.f;

	pFx->Set_UISizeAndPos(fxW, fxH, base.x + offX, base.y + offY);
	pFx->RegisterTexture(L"Com_Texture_PistolEff", L"Prototype_Component_Texture_WapPistol_Eff", 0, 10, 20.f, false);
	pFx->ChangeTexture(L"Com_Texture_PistolEff");

	pFx->Set_ObjTag(L"Eff");

	Add_Child(pFx);
}

void CShot_Gun::DeleteEff()
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag(TEXT("Eff")));
	if (!pEff)
		return;

	pEff->Set_Dead(true);
	Remove_Child(pEff);
}

CShot_Gun* CShot_Gun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShot_Gun* pInstance = new CShot_Gun(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CShot_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CShot_Gun::Clone(void* pArg)
{
	CShot_Gun* pInstance = new CShot_Gun(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CShot_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShot_Gun::Free()
{
	__super::Free();
}