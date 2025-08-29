#include "pch.h"
#include "CKatana.h"

CKatana::CKatana(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMainWeapon(pGraphicDev)
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

	Set_Texture();

	return S_OK;
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
		if (FAILED(Change_Texture(TEXT("Com_Texture_Sniper_Op"))))
			return E_FAIL;
		Set_UISizeAndPos(1014.f, 903.f, WINCX * 0.5f, WINCY * 0.5f + 100.f);
		break;
	}

	return S_OK;
}

HRESULT CKatana::Texture_Clone()
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
	__super::Free();
}