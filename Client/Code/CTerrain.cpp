#include "pch.h"
#include "CTerrain.h"
#include "CRenderer.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrain::CTerrain(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}


HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// Transform
	CTransform::TRANSFORMINFO		TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);
	TransformInfo.vStartPos = *(_vec3*)pArg;
	m_pTransformCom->SetTransformInfo(TransformInfo);

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}


_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	SetUp_TerrainY();
	return NO_EVENT;
}

void CTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
}

void CTerrain::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTerrain::Set_Component(void* pArg)
{
	// Texture
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STAGE, L"Prototype_Component_Texture_Terrian", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// VIBUFFER
	if (FAILED(Add_Components(L"Com_VIBuffer", SCENE_LOADING, L"Proto_TerrianBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CTerrain::SetUp_TerrainY()
{

}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pTerrain = new CTerrain(pGraphicDev);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("Terrain Create Failed");
	}

	return pTerrain;
}

CTerrain* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pTerrian Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	Engine::CGameObject::Free();
}
