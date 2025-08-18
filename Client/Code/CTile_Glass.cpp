#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CTexture.h"
#include "CTile_Glass.h"

CTile_Glass::CTile_Glass(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::GLASS)
{
}

CTile_Glass::CTile_Glass(const CTile_Glass &rhs)
    : CTileBase(rhs, TileType::GLASS)
{
}

CTile_Glass::~CTile_Glass()
{
}

void CTile_Glass::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Glass::Clone(void *pArg)
{
    CTile_Glass *pClone = new CTile_Glass(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Glass::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Glass *CTile_Glass::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Glass *pProto = new CTile_Glass(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Glass::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Glass::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Glass::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Glass::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Glass::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Glass::Render_GameObject()
{
    __super::Render_GameObject();
}

HRESULT CTile_Glass::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Glass", (CComponent **)&m_pTexture)))
        return E_FAIL;
    
    // TODO - Player의 공격에 충돌되면 개박살나는 Trigger Component
    // TODO - Effect Component

    return S_OK;
}
