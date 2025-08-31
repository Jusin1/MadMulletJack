#include "pch.h"
#include "CMan_HpBarUI.h"

CMan_HpBarUI::CMan_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

CMan_HpBarUI::CMan_HpBarUI(const CMan_HpBarUI& rhs)
	:CUI(rhs)
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

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	Set_UISizeAndPos(96.f, 94.5f,0.f,0.f);

	//timer 할래말래
	
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
	return;
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

HRESULT CMan_HpBarUI::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// ManN
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	texInfo = { 0,0,16,5.f,true };
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManN", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManN", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManN"), m_pTextureCom });

	// ManH
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManH", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManH", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManH"), m_pTextureCom });

	// ManD
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 16;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_ManD", SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManD", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_ManD"), m_pTextureCom });

	return S_OK;
}

HRESULT CMan_HpBarUI::Set_Texture(_int _iHitCount)
{
	//Set_Origin_Rot();

	m_bRenderOn = true;

	// 3이상이면 manD texture로 설정
	if (_iHitCount > 3)
	{
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_ManD"))))
			return E_FAIL;

		return S_OK;
	}
		
	
	// ihitcount으로 texture 바꾸고
	switch (_iHitCount)
	{
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
		break;
	}

	return S_OK;
}

HRESULT CMan_HpBarUI::Set_Texture(SCENE _eSCENE)
{
	// 만약 회전이 됐으면 다시 되돌려라
	Set_Origin_Rot();

	m_bRenderOn = true;

	// scene으로 transinfo 맞춤 
	switch (_eSCENE)
	{
	case SCENE_DEV:
	//case SCENE_TUTORIAL:
	case SCENE_STAGE_1:
	//case SCENE_STAGE_2:		
		m_vLocalOffset = { 22.f, -110.f,0.f };

		Set_New_TransInfo(50.f, 7.f);
		m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
		m_fRotSum += D3DXToRadian(7.f) * 1;

		break;

	case SCENE_STAGE_2: //test
	case SCENE_TUTORIAL:
	case SCENE_SNIPE:
	case SCENE_BOSS:
	case SCENE_CAR:
		m_vLocalOffset = { 30.f, -200.f,0.f };

		Set_New_TransInfo(50.f, 7.f);
		m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
		m_fRotSum += D3DXToRadian(7.f) * 1;
		break;

	case SCENE_END:
		m_vLocalOffset = { 22.f, -110.f,0.f };

		Set_New_TransInfo(50.f, 7.f);
		m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
		m_fRotSum += D3DXToRadian(7.f) * 1;
		break;

	default:
		m_bRenderOn = false;
	}

	return S_OK;
}

HRESULT CMan_HpBarUI::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	//m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

HRESULT CMan_HpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

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
