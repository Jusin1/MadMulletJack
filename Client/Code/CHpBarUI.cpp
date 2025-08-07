#include "pch.h"
#include "CHpBarUI.h"
#include "VIBuffer_Color.h"
#include "CDInputMgr.h"

CHpBarUI::CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_fHPPercent(1.f)
{
}

CHpBarUI::CHpBarUI(const CHpBarUI& rhs)
	:CUI(rhs), m_fHPPercent(rhs.m_fHPPercent)
{

}

CHpBarUI::~CHpBarUI()
{

}

HRESULT	CHpBarUI::Ready_GameObject()
{

	return S_OK;
}

HRESULT		CHpBarUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치 표시(핸드폰 UI에 고정하는 것으로 변경 예정)
	m_fSizeX = 50.0f;
	m_fSizeY = 70.0f;

	m_fX = 130.f;
	m_fY = 130.f;

	// CUI 컴포넌트 접근
	if(FAILED(Set_Component()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(m_fSizeX , m_fSizeY , 1.f);
	
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX - WINCX * 0.5f, -(m_fY - WINCY * 0.5f), 0.f));

	return S_OK;
}

_int		CHpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	// 체력 비율에 따라 조절
	__super::Update_GameObject(fTimeDelta);

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY * fTimeDelta, 1.f);

	return NO_EVENT;
}

void		CHpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//Key_UI(fTimeDelta);
	__super::LateUpdate_GameObject(fTimeDelta);
}
// 컬러로 렌더
void		CHpBarUI::Render_GameObject()
{
	m_pTransformCom->Apply_WorldMatrix();

	_matrix ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pColBufferCom->Render_Buffer();
}

void CHpBarUI::Key_UI(const _float& fTimeDelta)
{
	// 생명 및 시간 추가
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_O))
	{
		
	}
	// 생명 및 시간 감소
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_P))
	{
		m_fHPPercent -= 3.0f;
	}
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

void CHpBarUI::Free()
{
	Engine::CGameObject::Free();
}

// 체력 상호작용 받아오기
void CHpBarUI::Set_HpPersent(_float m_fHPPercent)
{

}
