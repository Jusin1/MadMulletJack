#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CTexture.h"
#include "CTile_Electric.h"

CTile_Electric::CTile_Electric(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::ELECTRIC)
{
}

CTile_Electric::CTile_Electric(const CTile_Electric &rhs)
    : CTileBase(rhs, TileType::ELECTRIC)
{
}

CTile_Electric::~CTile_Electric()
{
}

void CTile_Electric::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Electric::Clone(void *pArg)
{
    CTile_Electric *pClone = new CTile_Electric(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Electric::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Electric *CTile_Electric::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Electric *pProto = new CTile_Electric(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Electric::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Electric::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Electric::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Electric::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Electric::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Electric::Render_GameObject()
{
    __super::Render_GameObject();
}

HRESULT CTile_Electric::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Electric_Wall", (CComponent **)&m_pTexture)))
        return E_FAIL;

    // TODO - Monster와 충돌시 Trigger Component

    return S_OK;
}
