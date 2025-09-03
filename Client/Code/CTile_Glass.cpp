#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CVIBuffer_Rect.h"
#include "CPickingManager.h"
#include "CObjectPoolManager.h"
#include "CEffect_Pixel_Sprite.h"
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
        MSG_BOX("CTile_Glass::Create, Failed");
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

    CPickingManager::GetInstance()->Remove_PickingGroup(this);
    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Glass::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    CPickingManager::GetInstance()->Add_PickingGroup(this);
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Glass::Render_GameObject()
{
    __super::Render_GameObject();
}

_bool CTile_Glass::Picking(_vec3 *PickingPoint)
{
    return m_pBuffer->Picking(m_pTransformCom, PickingPoint);
}

void CTile_Glass::PickingTrue()
{
    Spawn_DestroyEffect();
    Set_Dead(TRUE);
}

HRESULT CTile_Glass::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Glass", (CComponent **)&m_pTexture)))
        return E_FAIL;
    
    // TODO - Effect Component

    return S_OK;
}

void CTile_Glass::Spawn_DestroyEffect()
{
    _vec3 vPosition = GetTransform()->Get_Info(INFO::INFO_POS);

    _float fRand = Rand_Float(0.1f, 0.4f);
    _float fRand2 = Rand_Float(0.1f, 0.4f);
    _float fRand3 = Rand_Float(0.1f, 0.4f);
    _float fRand4 = Rand_Float(0.1f, 0.4f);
    _float fRand5 = Rand_Float(0.1f, 0.4f);
    _float fRand6 = Rand_Float(0.1f, 0.4f);
    _float fRand7 = Rand_Float(0.1f, 0.4f);
    _float fRand8 = Rand_Float(0.1f, 0.4f);
    SpriteParticleOptions Option2;
    Option2.tEffectOption = Get_Preset_Blood();
    Option2.eType = SpriteParticleType::GLASS;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand, fRand2](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{fRand, -fRand2, 0});
        });
    Option2.eType = SpriteParticleType::GLASS;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand3, fRand4](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{-fRand3, fRand4, 0});
        });
    Option2.eType = SpriteParticleType::GLASS;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [&vPosition](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition);
        });
    Option2.eType = SpriteParticleType::GLASS;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand5, fRand6](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{ -fRand5, -fRand6, 0 });
        });
    Option2.eType = SpriteParticleType::GLASS;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand7, fRand8](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{ fRand7, fRand8, 0 });
        });
}
