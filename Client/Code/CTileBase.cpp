#include "pch.h"
#include "Engine_Define.h"
#include "CRenderer.h"
#include "CPickingManager.h"
#include "CVIBuffer_Rect.h"
#include "CTexture.h"
#include "CTileBase.h"

CTileBase::CTileBase(LPDIRECT3DDEVICE9 pGraphicDevice, TileType _e)
	: Engine::CGameObject(pGraphicDevice), m_pTexture(nullptr), m_pBuffer(nullptr), m_eType(_e)
	, m_eRenderID(RENDERID::RENDER_END), m_eCategory(ObjectCategory::TILE)
{
}

CTileBase::CTileBase(const CTileBase &rhs, TileType _e)
	: Engine::CGameObject(rhs), m_pTexture(nullptr), m_pBuffer(nullptr), m_eType(_e)
	, m_eRenderID(RENDERID::RENDER_END), m_eCategory(ObjectCategory::TILE)
{
}

CTileBase::~CTileBase()
{
}

void CTileBase::Free()
{
	Engine::CGameObject::Free();
	CPickingManager::GetInstance()->Remove_PickingGroup(this);
}

CGameObject *CTileBase::Clone(void *pArg)
{
	MSG_BOX("CTileBase::Clone, You tried to clone an invalid instance");
	return nullptr;
}

HRESULT CTileBase::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

HRESULT CTileBase::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Apply_WorldMatrix();

	return S_OK;
}

_int CTileBase::Update_GameObject(const _float &fTimeDelta)
{
	//CPickingManager::GetInstance()->Remove_PickingGroup(this);

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CTileBase::LateUpdate_GameObject(const _float &fTimeDelta)
{
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
	//CPickingManager::GetInstance()->Add_PickingGroup(this);
}

void CTileBase::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTileBase::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent **)&m_pBuffer), pData))
				return E_FAIL;

			GetTransform()->Set_Info(INFO::INFO_RIGHT, pData->transform.Right);
			GetTransform()->Set_Info(INFO::INFO_UP, pData->transform.Up);
			GetTransform()->Set_Info(INFO::INFO_LOOK, pData->transform.Look);
			GetTransform()->Set_Info(INFO::INFO_POS, pData->transform.Pos);
			GetTransform()->Apply_WorldMatrix();
		}
		else
		{
			MSG_BOX("CTileBase::Set_Component, Something wrong");
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("CTileBase::Set_Component, No Data");
		return E_FAIL;
	}

	return S_OK;
}
