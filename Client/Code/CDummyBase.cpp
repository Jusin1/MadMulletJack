#include "pch.h"
#include "Engine_Define.h"
#include "CRenderer.h"
#include "CPickingManager.h"
#include "CVIBuffer_Rect.h"
#include "CTexture.h"
#include "CDummyBase.h"

CDummyBase::CDummyBase(LPDIRECT3DDEVICE9 pGraphicDevice, EnvType _e)
	: Engine::CGameObject(pGraphicDevice), m_pCollider(nullptr), m_eType(_e)
	, m_eRenderID(RENDERID::RENDER_END), m_eCategory(ObjectCategory::ENV_OBJ)
{
}

CDummyBase::CDummyBase(const CDummyBase &rhs, EnvType _e)
	: Engine::CGameObject(rhs), m_pCollider(nullptr), m_eType(_e)
	, m_eRenderID(RENDERID::RENDER_END), m_eCategory(ObjectCategory::ENV_OBJ)
{
}

CDummyBase::~CDummyBase()
{
}

void CDummyBase::Free()
{
	Engine::CGameObject::Free();
}

CGameObject *CDummyBase::Clone(void *pArg)
{
	MSG_BOX("CDummyBase::Clone, You tried to clone an invalid instance");
	return nullptr;
}

HRESULT CDummyBase::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

HRESULT CDummyBase::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CDummyBase::Update_GameObject(const _float &fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CDummyBase::LateUpdate_GameObject(const _float &fTimeDelta)
{
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDummyBase::Render_GameObject()
{
}

HRESULT CDummyBase::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			GetTransform()->Set_Info(INFO::INFO_RIGHT, pData->transform.Right);
			GetTransform()->Set_Info(INFO::INFO_UP, pData->transform.Up);
			GetTransform()->Set_Info(INFO::INFO_LOOK, pData->transform.Look);
			GetTransform()->Set_Info(INFO::INFO_POS, pData->transform.Pos);
			GetTransform()->Apply_WorldMatrix();
		}
		else
		{
			MSG_BOX("CDummyBase::Set_Component, Something wrong");
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("CDummyBase::Set_Component, No Data");
		return E_FAIL;
	}

	return S_OK;
}
