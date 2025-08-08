#include "pch.h"
#include "CSkyBox.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CSkyBox::CSkyBox(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CSkyBox::~CSkyBox()
{
}

HRESULT CSkyBox::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// Transform
	CTransform::TRANSFORMINFO		TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);

	if (FAILED(Set_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(40.f, 40.f, 40.f);

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	_matrix	matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	m_pTransformCom->Set_Info(INFO_POS, _vec3(matView._41, matView._42 + 3.f, matView._43));

	return 0;
}

void CSkyBox::LateUpdate_GameObject(const _float& fTimeDelta)
{

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_PRIORITY, this);
}

void CSkyBox::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 	
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(3);

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSkyBox::Set_Component()
{
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STAGE, L"Prototype_Component_Texture_SkyBox", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(Add_Components(L"Com_VIBuffer", SCENE_LOADING, L"Proto_CubeBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}


CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox* pPlayer = new CSkyBox(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pSky Create Failed");
		return nullptr;
	}

	return pPlayer;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox* pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pSky Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	Engine::CGameObject::Free();
}
