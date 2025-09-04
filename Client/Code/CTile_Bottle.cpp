#include "pch.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CObjectPoolManager.h"
#include "CEffect_Pixel_Sprite.h"
#include "CVIBuffer_Rect.h"
#include "CPickingManager.h"
#include "CTexture.h"
#include "CTile_Bottle.h"
#include "Sound_Manager.h"

CTile_Bottle::CTile_Bottle(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::BOTTLE)
{
}

CTile_Bottle::CTile_Bottle(const CTile_Bottle &rhs)
    : CTileBase(rhs, TileType::BOTTLE)
{
}

CTile_Bottle::~CTile_Bottle()
{
}

void CTile_Bottle::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Bottle::Clone(void *pArg)
{
    CTile_Bottle *pClone = new CTile_Bottle(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Bottle::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Bottle *CTile_Bottle::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Bottle *pProto = new CTile_Bottle(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("CTile_Bottle::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Bottle::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Bottle::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Bottle::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    CPickingManager::GetInstance()->Remove_PickingGroup(this);
    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Bottle::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    SetUp_BillBoard();
    CPickingManager::GetInstance()->Add_PickingGroup(this);
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Bottle::Render_GameObject()
{
    __super::Render_GameObject();
}

_bool CTile_Bottle::Picking(_vec3 *PickingPoint)
{
    return m_pBuffer->Picking(m_pTransformCom, PickingPoint);
}

void CTile_Bottle::PickingTrue()
{
    //"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\object\glass\glass.destroy-003.wav"
    CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/object/glass/glass.destroy-003.wav", SOUND_OBJECT, 2.f, false);
    Spawn_DestroyEffect();
    Set_Dead(TRUE);
}

void CTile_Bottle::Spawn_DestroyEffect()
{
    _vec3 vPosition = GetTransform()->Get_Info(INFO::INFO_POS);
    _float fRand = Rand_Float(0.1f, 0.4f);
    _float fRand2 = Rand_Float(0.1f, 0.4f);
    _float fRand3 = Rand_Float(0.1f, 0.4f);
    _float fRand4 = Rand_Float(0.1f, 0.4f);    
    SpriteParticleOptions Option2;
    Option2.tEffectOption = Get_Preset_Blood();
    Option2.eType = SpriteParticleType::BOTTLE;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand, fRand2](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{ fRand, -fRand2, 0 });
        });
    Option2.eType = SpriteParticleType::BOTTLE;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPosition, fRand3, fRand4](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition + _vec3{ -fRand3, fRand4, 0 });
        });
    Option2.tEffectOption = Get_Preset_BulletSpark();
    Option2.tEffectOption.fDrag = 1.f;
    Option2.tEffectOption.fSpeed_Min = 4.f;
    Option2.tEffectOption.fSpeed_Max = 7.f;
    Option2.eType = SpriteParticleType::BOTTLE;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [&vPosition](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPosition);
        });
}

void CTile_Bottle::SetUp_BillBoard()
{
    _matrix _matView;

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &_matView);
    D3DXMatrixInverse(&_matView, nullptr, &_matView);

    _vec3 vRight = *(_vec3 *)&_matView.m[0][0];
    _vec3 vUp = *(_vec3 *)&_matView.m[1][0];
    _vec3 vLook = *(_vec3 *)&_matView.m[2][0];
    m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
    m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
    m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}

HRESULT CTile_Bottle::Set_Component(void *pArg)
{
    if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
    {
        if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, pData->texture.OriginComponentName.c_str(), (CComponent **)&m_pTexture)))
            return E_FAIL;

        m_pTransformCom->Set_Scale(0.125f, 0.25f, 1.f);
        _vec3 src = m_pTransformCom->Get_Info(INFO_POS);
        src.y += m_pTransformCom->Get_Scale().y * 0.5f;
        m_pTransformCom->Set_Info(INFO_POS, src);
        // TODO - Effect Component
    }

    return S_OK;
}
