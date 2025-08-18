#include "pch.h"
#include "CColRect_HpBarUI.h"

CColRect_HpBarUI::CColRect_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

CColRect_HpBarUI::CColRect_HpBarUI(const CColRect_HpBarUI& rhs)
	:CUI(rhs)
{
}

CColRect_HpBarUI::~CColRect_HpBarUI()
{
}

HRESULT	CColRect_HpBarUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CColRect_HpBarUI::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//timer 할래말래

	Set_UISizeAndPos(420.f, 900.f, WINCX * 0.5f - 450.f, WINCY * 0.5f + 400.f);
	Set_New_TransInfo(50.f, 0.f);

	return S_OK;
}

_int CColRect_HpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CColRect_HpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CColRect_HpBarUI::Render_GameObject()
{
	m_pVIColBufferCom->Render_Buffer();

	__super::Render_GameObject();
}

HRESULT CColRect_HpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

	// color VIBUFFER
	if (FAILED(Add_Components(L"Com_VIColorBuffer", SCENE_STATIC, L"Proto_Color_Buffer", (CComponent**)&m_pVIColBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CColRect_HpBarUI::Set_HpBarColor(_float _fPercent)
{
	// percent 에 따라 색깔 (R:1-percent, G : percent , B =0)
	D3DXCOLOR tColor = D3DXCOLOR(1.f-_fPercent,_fPercent, 0.f,1.f);

	m_pVIColBufferCom->SetColor(tColor);
}

CColRect_HpBarUI* CColRect_HpBarUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CColRect_HpBarUI* pHpBarRect = new CColRect_HpBarUI(pGraphicDev);

	if (FAILED(pHpBarRect->Ready_GameObject()))
	{
		Safe_Release(pHpBarRect);
		MSG_BOX("CColRect_HpBarUI Create Failed");
		return nullptr;
	}

	return pHpBarRect;
}

CGameObject* CColRect_HpBarUI::Clone(void* pArg)
{
	CColRect_HpBarUI* pInstance = new CColRect_HpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CColRect_HpBarUI Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CColRect_HpBarUI::Free()
{
	__super::Free();
}