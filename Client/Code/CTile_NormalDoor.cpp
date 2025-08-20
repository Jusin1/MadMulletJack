#include "pch.h"
#include "CTile_NormalDoor.h"

CTile_NormalDoor::CTile_NormalDoor(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CTileBase(pGraphicDevice, TileType::NORMALDOOR)
{
}

CTile_NormalDoor::CTile_NormalDoor(const CTile_NormalDoor &rhs)
	: CTileBase(rhs, TileType::NORMALDOOR)
{
}

CTile_NormalDoor::~CTile_NormalDoor()
{
}

void CTile_NormalDoor::Free()
{
	CTileBase::Free();
}

CGameObject *CTile_NormalDoor::Clone(void *pArg)
{
	return nullptr;
}

CTile_NormalDoor *CTile_NormalDoor::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	return nullptr;
}

HRESULT CTile_NormalDoor::Ready_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CTile_NormalDoor::Initialize(void *pArg)
{
	return E_NOTIMPL;
}

_int CTile_NormalDoor::Update_GameObject(const _float &fTimeDelta)
{
	return _int();
}

void CTile_NormalDoor::LateUpdate_GameObject(const _float &fTimeDelta)
{
}

void CTile_NormalDoor::Render_GameObject()
{
}

HRESULT CTile_NormalDoor::Set_Component(void *pArg)
{
	return E_NOTIMPL;
}
