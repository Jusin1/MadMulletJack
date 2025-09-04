#include "pch.h"
#include "CCamera.h"
#include "CManagement.h"
#include "CMapFactory.h"
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
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	CTransform::TRANSFORMINFO		TransformInfo;
	::ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));
	TransformInfo.fSpeed = 5.f;

	if (FAILED(Set_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	_matrix	matView = CCamera::GetView();
	D3DXMatrixInverse(&matView, 0, &matView);

	if (CManagement::GetInstance()->Get_CurrentSceneIdx() == SCENE_SNIPE)
	{
		m_pTransformCom->Set_Info(INFO_POS, _vec3(matView._41, matView._42 + 0.54f, matView._43));
	}
	else
	{
		m_pTransformCom->Set_Info(INFO_POS, _vec3(matView._41, matView._42 + 0.93f, matView._43));
	}

	return 0;
}

void CSkyBox::LateUpdate_GameObject(const _float& fTimeDelta)
{

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_PRIORITY, this);
}

void CSkyBox::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 	
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSkyBox::Set_Component()
{
	_uint iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();

	if (iSceneIndex == SCENE_BOSS || iSceneIndex == SCENE_SNIPE)
	{
		if (FAILED(Add_Components(L"Com_Texture", iSceneIndex, L"Prototype_Component_Texture_SkyBox", (CComponent **)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (iSceneIndex == SCENE_CAR)
	{
		if (FAILED(Add_Components(L"Com_Texture", iSceneIndex, L"Prototype_Component_Texture_SkyBox2", (CComponent **)&m_pTextureCom)))
			return E_FAIL;
	}

	if (FAILED(Add_Components(L"Com_VIBuffer", SCENE_STATIC, L"Proto_CubeBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}


CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox* pSkybox = new CSkyBox(pGraphicDev);

	if (FAILED(pSkybox->Ready_GameObject()))
	{
		Safe_Release(pSkybox);
		MSG_BOX("pSky Create Failed");
		return nullptr;
	}

	return pSkybox;
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
