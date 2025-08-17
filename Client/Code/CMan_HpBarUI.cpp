#include "pch.h"
#include "CMan_HpBarUI.h"

CMan_HpBarUI::CMan_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CHpBarUI(pGraphicDev)
{
}

CMan_HpBarUI::CMan_HpBarUI(const CMan_HpBarUI& rhs)
	:CHpBarUI(rhs)
{
}

CMan_HpBarUI::~CMan_HpBarUI()
{
}

HRESULT	CMan_HpBarUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMan_HpBarUI::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//timer 할래말래

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	HitCount_Reset(); // hitcount <- 0 (scene 전환시 0으로 맞추기 위함)

	Set_UISizeAndPos(420.f, 900.f,WINCX * 0.5f - 450.f, WINCY * 0.5f + 400.f);

	return S_OK;
}

_int CMan_HpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMan_HpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 hitcount가 변화했다면
	if (m_bHitChange) {
		// texture를 새로 셋팅하고
		Set_Texture();
		// 다시 false로 바꿈
		m_bHitChange = false;
	}
}

void CMan_HpBarUI::Render_GameObject()
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

HRESULT CMan_HpBarUI::Set_Texture()
{
	// 만약 이 전에 돌려놨다면
	if (m_fRotSum != 0)
	{
		// 다시 원상복귀
		Set_Origin_Rot();
	}

	m_bRenderOn = true;

	switch (m_iHitCount) {
	case 0:
	case 1:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_ManN"))))
			return E_FAIL;

		break;

	case 2:
	case 3:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_ManH"))))
			return E_FAIL;

		break;

	default:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_ManD"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMan_HpBarUI::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

HRESULT CMan_HpBarUI::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// ManN
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManN", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManN", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManN"), m_pTextureCom });

	// ManH
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManH", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManH", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManH"), m_pTextureCom });

	// ManD
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManD", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManD", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManD"), m_pTextureCom });

	return S_OK;
}

HRESULT CMan_HpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;
	//if (FAILED(Add_Components(L"Com_Color", SCENE_STAGE, L"Proto_Color_Buffer", (CComponent**)&m_pColBufferCom)))
	//	return E_FAIL;

	return S_OK;
}

CMan_HpBarUI* CMan_HpBarUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMan_HpBarUI* pHpBarMan = new CMan_HpBarUI(pGraphicDev);

	if (FAILED(pHpBarMan->Ready_GameObject()))
	{
		Safe_Release(pHpBarMan);
		MSG_BOX("CMan_HpBarUI Create Failed");
		return nullptr;
	}

	return pHpBarMan;
}

CGameObject* CMan_HpBarUI::Clone(void* pArg)
{
	CMan_HpBarUI* pInstance = new CMan_HpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMan_HpBarUI Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CMan_HpBarUI::Free()
{
	__super::Free();
}
