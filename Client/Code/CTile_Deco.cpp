#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CTexture.h"
#include "CVIBuffer_Rect.h"
#include "CTile_Deco.h"

CTile_Deco::CTile_Deco(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::DECO)
{
}

CTile_Deco::CTile_Deco(const CTile_Deco &rhs)
    : CTileBase(rhs, TileType::DECO)
{
}

CTile_Deco::~CTile_Deco()
{
}

void CTile_Deco::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Deco::Clone(void *pArg)
{
    CTile_Deco *pClone = new CTile_Deco(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Deco::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Deco *CTile_Deco::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Deco *pProto = new CTile_Deco(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Deco::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Deco::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Deco::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Deco::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Deco::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Deco::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();
    m_pTexture->Set_Texture(m_iTextureIndex);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pBuffer->Render_Buffer();

    // ¿ø»óº¹±Í
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTile_Deco::Set_Component(void *pArg)
{
    if (pArg)
    {
        if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
        {
            if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, pData->texture.OriginComponentName.c_str(), (CComponent **)&m_pTexture), pData))
                return E_FAIL;
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
