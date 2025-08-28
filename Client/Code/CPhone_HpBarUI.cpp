#include "pch.h"
#include "CPhone_HpBarUI.h"
#include "CManagement.h"

CPhone_HpBarUI::CPhone_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

CPhone_HpBarUI::CPhone_HpBarUI(const CPhone_HpBarUI& rhs)
	:CUI(rhs)
{
}

CPhone_HpBarUI::~CPhone_HpBarUI()
{
}

HRESULT	CPhone_HpBarUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhone_HpBarUI::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	//timer 할래말래
	Set_UISizeAndPos(420.f, 900.f, 0.f, 0.f);

	return S_OK;
}

_int CPhone_HpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);
	return NO_EVENT;
}

void CPhone_HpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CPhone_HpBarUI::Render_GameObject()
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

HRESULT CPhone_HpBarUI::Set_Texture(SCENE _eSCENE)
{
	Set_Origin_Rot();

	m_bRenderOn = true;

	m_tMoveInfo.fRange = 10.f;

	switch (_eSCENE)
	{
	case SCENE_DEV:
	//case SCENE_TUTORIAL:
	case SCENE_STAGE_1:
	case SCENE_STAGE_2:
	case SCENE_STAGE_3:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_PhoneN"))))
			return E_FAIL;
		break;

	case SCENE_TUTORIAL:
	case SCENE_SNIPE:
	case SCENE_BOSS:
	case SCENE_CAR:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_PhoneB"))))
			return E_FAIL;
		break;

	case SCENE_END:
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_PhoneF"))))
			return E_FAIL;
		break;

	default:
		m_bRenderOn = false;
	}

	return S_OK;
}

HRESULT CPhone_HpBarUI::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// PhoneN
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneN", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneN", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneN"), m_pTextureCom });

	// PhoneB
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneB", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneB", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneB"), m_pTextureCom });

	// PhoneF
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneF", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneF", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneF"), m_pTextureCom });

	return S_OK;
}

HRESULT CPhone_HpBarUI::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

HRESULT CPhone_HpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

	return S_OK;
}

CPhone_HpBarUI* CPhone_HpBarUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPhone_HpBarUI* pHpBarMan = new CPhone_HpBarUI(pGraphicDev);

	if (FAILED(pHpBarMan->Ready_GameObject()))
	{
		Safe_Release(pHpBarMan);
		MSG_BOX("CPhone_HpBarUI Create Failed");
		return nullptr;
	}

	return pHpBarMan;
}

CGameObject* CPhone_HpBarUI::Clone(void* pArg)
{
	CPhone_HpBarUI* pInstance = new CPhone_HpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPhone_HpBarUI Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CPhone_HpBarUI::Free()
{
	__super::Free();
}
