#include "pch.h"
#include "CHpBarUI.h"
#include "VIBuffer_Color.h"
#include "CDInputMgr.h"
#include "CManagement.h"

CHpBarUI::CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_fHpPercent(1.f), m_eScene(SCENE_END)
	, m_pColBufferCom(nullptr), m_bHitChange(false)
{
}

CHpBarUI::CHpBarUI(const CHpBarUI& rhs)
	:CUI(rhs), m_fHpPercent(rhs.m_fHpPercent), m_eScene(rhs.m_eScene)
	, m_pColBufferCom(rhs.m_pColBufferCom) , m_bHitChange(rhs.m_bHitChange)
{

}

CHpBarUI::~CHpBarUI()
{

}

HRESULT	CHpBarUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT		CHpBarUI::Initialize(void* pArg)
{
	//D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//timer 할래말래

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bReanderFront = true; // 손 보다 위에 그리기 위해

	return S_OK;
}

_int		CHpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	// 체력 비율에 따라 조절
	__super::Update_GameObject(fTimeDelta);
	//m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY * fTimeDelta, 1.f);

	return NO_EVENT;
}

void		CHpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	if (m_eScene != (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx()) // scene 이 바뀌면 
	{
		m_eScene = (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx(); 
		Set_Texture(); // texture를 바꿔라
	}
}
// 컬러로 렌더
void		CHpBarUI::Render_GameObject()
{
	//m_pColBufferCom->Render_Buffer(); //hp 색깔 buffet render

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

HRESULT CHpBarUI::Set_Texture()
{
	// 만약 이 전에 돌려놨다면
	if (m_fRotSum != 0)
	{
		// 다시 원상복귀
		Set_Origin_Rot();
	}

	m_bRenderOn = true;

	switch (m_eScene) {
	case SCENE_STAGE:
	{
		if(FAILED(Change_Texture(TEXT("Com_Texture_HpBar_PhoneN"))))
			return E_FAIL;

		Set_UISizeAndPos(420.f, 900.f,WINCX * 0.5f - 450.f, WINCY * 0.5f + 400.f); // 왼쪽은 고정
	}
		break;

	case SCENE_BOSS:
	{
		if (FAILED(Change_Texture(TEXT("Com_Texture_HpBar_PhoneB"))))
			return E_FAIL;
	}
	break;

	default:
		m_bRenderOn = false;
	}
}

HRESULT CHpBarUI::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

HRESULT CHpBarUI::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// PhoneN
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneN", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneN", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneN"), m_pTextureCom });

	// PhoneB
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneB", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneB", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneB"), m_pTextureCom });

	// PhoneF
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 4;
	texInfo.m_fSpeed = 5.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_HpBar_PhoneF", SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneF", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_HpBar_PhoneF"), m_pTextureCom });


	return S_OK;
}

CHpBarUI* CHpBarUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHpBarUI* pHpBar = new CHpBarUI(pGraphicDev);

	if (FAILED(pHpBar->Ready_GameObject()))
	{
		Safe_Release(pHpBar);
		MSG_BOX("CHpBarUI Create Failed");
		return nullptr;
	}

	return pHpBar;
}

CGameObject* CHpBarUI::Clone(void* pArg)
{
	CHpBarUI* pInstance = new CHpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHpBarUI Clone Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CHpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;
	//if (FAILED(Add_Components(L"Com_Color", SCENE_STAGE, L"Proto_Color_Buffer", (CComponent**)&m_pColBufferCom)))
	//	return E_FAIL;

	return S_OK;
}

// 체력 상호작용 받아오기
void CHpBarUI::Set_Hp(_float _fMaxHp, _float _fCurHp)
{
	// 내가 그리는 y값의 percent 만큼 그리기

	m_fHpPercent = _fCurHp / _fMaxHp;

	// percent 에 따라 색깔 (R:1-percent, G : percent , B =0)

	// curhp에 따라 출력 글씨 셋팅
}

void CHpBarUI::Free()
{
	__super::Free();
}