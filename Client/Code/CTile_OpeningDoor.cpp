#include "pch.h"
#include "CTile_OpeningDoor.h"

CTile_OpeningDoor::CTile_OpeningDoor(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CTileBase(pGraphicDevice, TileType::OPENINGDOOR)
{
}

CTile_OpeningDoor::CTile_OpeningDoor(const CTile_OpeningDoor &rhs)
	: CTileBase(rhs, TileType::OPENINGDOOR)
{
}

CTile_OpeningDoor::~CTile_OpeningDoor()
{
}

void CTile_OpeningDoor::Free()
{
	CTileBase::Free();
}

CGameObject *CTile_OpeningDoor::Clone(void *pArg)
{
	CTile_OpeningDoor *pClone = new CTile_OpeningDoor(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CTile_OpeningDoor::Clone, Failed");
		Safe_Release(pClone);
	}

	return pClone;
}

CTile_OpeningDoor *CTile_OpeningDoor::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CTile_OpeningDoor *pProto = new CTile_OpeningDoor(pGraphicDevice);

	if (FAILED(pProto->Ready_GameObject()))
	{
		Safe_Release(pProto);
		MSG_BOX("CTile_OpeningDoor::Create, Failed");
		return nullptr;
	}

	return pProto;
}

HRESULT CTile_OpeningDoor::Ready_GameObject()
{
	return CTileBase::Ready_GameObject();
}

HRESULT CTile_OpeningDoor::Initialize(void *pArg)
{
	if (FAILED(CTileBase::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CTile_OpeningDoor::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return DEAD;

	return __super::Update_GameObject(fTimeDelta);
}

void CTile_OpeningDoor::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return;

	__super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_OpeningDoor::Render_GameObject()
{
	__super::Render_GameObject();
}

HRESULT CTile_OpeningDoor::Set_Component(void *pArg)
{
	return S_OK;
}
