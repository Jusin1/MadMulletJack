#include "pch.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBackGround::CBackGround(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CBackGround::~CBackGround()
{

}

HRESULT CBackGround::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	m_pRenderCom->Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CBackGround::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
}

void CBackGround::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pTextureCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBackGround::Set_Component()
{
	// Render
	if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// Texture
	if (FAILED(Add_Components(L"Com_Texture", SCENE_LOGO, L"Prototype_Component_Texture_BackGround", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround* pBackGround = new CBackGround(pGraphicDev);

	if (FAILED(pBackGround->Ready_GameObject()))
	{
		Safe_Release(pBackGround);
		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pBackGround;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pBackGround Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	Engine::CGameObject::Free();
}
