#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CTexture.h"
#include "CTile_Acid.h"

CTile_Acid::CTile_Acid(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::ACID)
{
}

CTile_Acid::CTile_Acid(const CTile_Acid &rhs)
    : CTileBase(rhs, TileType::ACID)
{
}

CTile_Acid::~CTile_Acid()
{
}

void CTile_Acid::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Acid::Clone(void *pArg)
{
    CTile_Acid *pClone = new CTile_Acid(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Acid::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Acid *CTile_Acid::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Acid *pProto = new CTile_Acid(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Acid::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Acid::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Acid::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Acid::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Acid::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Acid::Render_GameObject()
{
    __super::Render_GameObject();
}

HRESULT CTile_Acid::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Acid_Floor", (CComponent **)&m_pTexture)))
        return E_FAIL;

    return S_OK;
}
