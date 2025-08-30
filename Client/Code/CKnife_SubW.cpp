#include "pch.h"
#include "CKnife_SubW.h"
#include "CTimerMgr.h"
#include "CObjectManager.h"
#include "CManagement.h"

CKnife_SubW::CKnife_SubW(LPDIRECT3DDEVICE9 pGraphicDev)
	:CSubWeapon(pGraphicDev)
{
}

CKnife_SubW::CKnife_SubW(const CKnife_SubW& rhs)
	:CSubWeapon(rhs)
{
}

CKnife_SubW::~CKnife_SubW()
{
}

HRESULT CKnife_SubW::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_SubWap_Knife"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKnife_SubW::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;

	return S_OK;
}

_int CKnife_SubW::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	// 만약 지금 attack texture 라면
	if (m_CurrentAnimTag == TEXT("Com_Texture_Knife_Att"))
	{
		// scene을 받아옴
		SCENE eCurScene = (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx();

		// handR을 받아옴
		CUIBase* pHandR = dynamic_cast<CUIBase*>(CObjectManager::GetInstance()
			->Find_Object(eCurScene, L"UI_Layer", 1))
			->Find_Child_ByTag(L"HandRUI");

		// handR 위치를 기준으로 pos 갱신 (offset 적용)
		if (pHandR)
		{
			Set_UIPos(pHandR->GetTransform()->Get_Info(INFO_POS), -250.f, 320.f);
		}
	}

	return NO_EVENT;
}

void CKnife_SubW::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CKnife_SubW::Render_GameObject()
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

HRESULT CKnife_SubW::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Knife_Idle", SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Idle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Knife_Idle"), m_pTextureCom });

	// Item
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 1;
	texInfo.m_fSpeed = 0.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Knife_Item", SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Item", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Knife_Item"), m_pTextureCom });

	// Attack
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 1;
	texInfo.m_fSpeed = 1.8f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Knife_Att", SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Att", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Knife_Att"), m_pTextureCom });

	return S_OK;
}

HRESULT CKnife_SubW::Set_Texture() 
{
	// 만약 이 전에 돌려놨다면
	if (m_fRotSum != 0)
	{
		// 다시 원상복귀
		Set_Origin_Rot();
	}

	m_bRenderOn = true;

	switch (m_eWapState)
	{
	case IDLE:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Knife_Idle"))))
			return E_FAIL;

		Set_UISizeAndPos(280.f, 500.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 150.f);

		break;

	case ITEM:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Knife_Item"))))
			return E_FAIL;

		Set_UISizeAndPos(280.f, 500.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 150.f);

		break;
	
	case WEAPON:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Knife_Att"))))
			return E_FAIL;

		Set_UISize(216.f, 460.f);

		Set_New_TransInfo(380.f, 40.f);
		m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
		m_fRotSum += D3DXToRadian(40.f) * 1;

		break;

	default:
		m_bRenderOn = false;
	}
	return S_OK;
}

HRESULT CKnife_SubW::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

CKnife_SubW* CKnife_SubW::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKnife_SubW* pInstance = new CKnife_SubW(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CKnife_SubW Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CKnife_SubW::Clone(void* pArg)
{
	CKnife_SubW* pInstance = new CKnife_SubW(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CKnife_SubW Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKnife_SubW::Free()
{
	__super::Free();
}