#include "pch.h"
#include "CKatana.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CImageUI.h"

CKatana::CKatana(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMainWeapon(pGraphicDev)
	, m_pOpenUI(nullptr)
{
}

CKatana::CKatana(const CKatana& rhs)
	: CMainWeapon(rhs)
{
}

CKatana::~CKatana()
{
}

HRESULT CKatana::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKatana::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
	m_pOpenUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
	if (!m_pOpenUI) return E_FAIL;
	// 화면 스크린 생성
	m_pSheathUI = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));


	if (m_pSheathUI)
	{
		Set_UISizeAndPos(400.f, 100.f, WINCX * 0.5f - 400.f, WINCY * 0.5f + 50.f);
		m_pSheathUI->RegisterTexture(L"Com_Texture_Sheath", L"Prototype_Component_Texture_KatanaSheath", 0, 1, 0.f, true);
		m_pSheathUI->ChangeTexture(L"Com_Texture_Sheath");
		m_pOpenUI->Add_Child(m_pSheathUI);
	}

	Set_Texture();
}

_int CKatana::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CKatana::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CKatana::Render_GameObject()
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

HRESULT CKatana::Set_Texture()
{

	m_tMoveInfo.eUIMove = MV_NON;

	switch (m_tInfo.ePlayerState)
	{
	case OPENING:
		break;
	}

	return S_OK;
}

HRESULT CKatana::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 1;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Sniper_Idle", SCENE_STATIC, L"Prototype_Component_Texture_KatanaSheath", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Sniper_Idle"), m_pTextureCom });
	return S_OK;
}

HRESULT CKatana::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

CKatana* CKatana::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKatana* pInstance = new CKatana(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSniper_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CKatana::Clone(void* pArg)
{
	CKatana* pInstance = new CKatana(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSniper_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKatana::Free()
{
	Safe_Release(m_pOpenUI);
	__super::Free();
}