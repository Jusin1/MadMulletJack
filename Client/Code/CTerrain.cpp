#include "pch.h"
#include "CTerrain.h"
#include "CProtoMgr.h"
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
	if (FAILED(Add_Component()))
		return E_FAIL;


	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTerrain::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTerrain::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer",pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

	return S_OK;
}

void CTerrain::Key_Input(const _float& fTimeDelta)
{
	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_LOOK, &vUp);

	if (GetAsyncKeyState(VK_UP))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vUp, &vUp), 5.f, fTimeDelta);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vUp, &vUp), -5.f, fTimeDelta);
	}


	if (GetAsyncKeyState('Q'))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(90.f * fTimeDelta));
	}

	if (GetAsyncKeyState('A'))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(90.f * -fTimeDelta));
	}
}



CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pTerrain = new CTerrain(pGraphicDev);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("Terrain Create Failed");
		return nullptr;
	}

	return pTerrain;
}

void CTerrain::Free()
{
	Engine::CGameObject::Free();
}
