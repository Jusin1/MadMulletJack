#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CTexture.h"
#include "CTile_Vent.h"

CTile_Vent::CTile_Vent(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::VENT), m_pProp(nullptr), m_pColliderSphere(nullptr)
{
}

CTile_Vent::CTile_Vent(const CTile_Vent &rhs)
    : CTileBase(rhs, TileType::VENT), m_pProp(nullptr), m_pColliderSphere(nullptr)
{
}

CTile_Vent::~CTile_Vent()
{
}

void CTile_Vent::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Vent::Clone(void *pArg)
{
    CTile_Vent *pClone = new CTile_Vent(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Vent::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Vent *CTile_Vent::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Vent *pProto = new CTile_Vent(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Vent::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Vent::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Vent::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Vent::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Vent::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Vent::Render_GameObject()
{
    __super::Render_GameObject();
}

HRESULT CTile_Vent::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Vent", (CComponent **)&m_pTexture)))
        return E_FAIL;

    if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
    {
        wstring originName = pData->texture.OriginComponentName;
        pData->texture.OriginComponentName = originName + L"_Prop";
        _matrix vTransformData;
        ::D3DXMatrixIdentity(&vTransformData);
        vTransformData._41 -= 0.25f;
        vTransformData *= (*m_pTransformCom->Get_World());
        ::memcpy(&pData->transform.Right[0], vTransformData.m[0], sizeof(_vec3));
        ::memcpy(&pData->transform.Up[0], vTransformData.m[1], sizeof(_vec3));
        ::memcpy(&pData->transform.Look[0], vTransformData.m[2], sizeof(_vec3));
        ::memcpy(&pData->transform.Pos[0], vTransformData.m[3], sizeof(_vec3));
        if (FAILED(CObjectManager::GetInstance()->Add_GameObject(
            L"Prototype_GameObject_DecoTile",
            CMapFactory::GetInstance()->GetTargetSceneIndex(),
            L"Tile_Layer",
            pData)))
            return E_FAIL;
    }
    

    // TODO - Monster와 충돌시 Trigger Component

    return S_OK;
}
