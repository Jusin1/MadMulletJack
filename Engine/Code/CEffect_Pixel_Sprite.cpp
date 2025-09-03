#include "Engine_Define.h"
#include "CEffect_Pixel.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CCullingManager.h"
#include "CEffect_Pixel_Sprite.h"

CEffect_Pixel_Sprite::CEffect_Pixel_Sprite(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEffect_Pixel_Sprite::CEffect_Pixel_Sprite(const CEffect_Pixel_Sprite &rhs)
	: CGameObject(rhs)
{
}

CEffect_Pixel_Sprite::~CEffect_Pixel_Sprite()
{
}

void CEffect_Pixel_Sprite::Free()
{
    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
	CGameObject::Free();
}

CEffect_Pixel_Sprite *CEffect_Pixel_Sprite::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Pixel_Sprite *pInstance = new CEffect_Pixel_Sprite(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CEffect_Pixel_Sprite Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject *CEffect_Pixel_Sprite::Clone(void *pArg)
{
    CEffect_Pixel_Sprite *pInstance = new CEffect_Pixel_Sprite(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CEffect_Pixel_Sprite Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CEffect_Pixel_Sprite::Ready_GameObject()
{
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Pixel_Sprite::Initialize(void *pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    m_iTextureCount = 20;

    SetOptions(EffectOptions{}, true);

    return S_OK;
}

_int CEffect_Pixel_Sprite::Update_GameObject(const _float &fTimeDelta)
{
    if (m_iAliveCount <= 0)
        return DEAD;

    Update_Position(m_pTransformCom->Get_Info(INFO_POS));
    Compute_CamDistance(Get_Position());

    SetUp_BillBoard();

    Particle_Update(fTimeDelta);

    if (CCullingManager::GetInstance()->Is_In_Frustum(Get_Position(), m_fRadius))
        if (m_pRendererCom) m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

    return NO_EVENT;
}

void CEffect_Pixel_Sprite::Render_GameObject()
{
    if (m_iAliveCount <= 0)
        return;

    if (FAILED(Ready_VB()))
    {
        MSG_BOX("CEffect_Pixel_Sprite::Render_GameObject, Failed");
        return;
    }
    m_pTransformCom->Apply_WorldMatrix();

    m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX));
    m_pGraphicDev->SetIndices(m_pIB);
    m_pGraphicDev->SetFVF(FVF_TEX);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    for (int i = 0; i < m_iAliveCount; ++i)
    {
        m_pTextureCom->Set_Texture(i % m_pTextureCom->Get_Frame().m_iEndTex);
        m_pGraphicDev->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST, i*4,
            0, 4,
            0, 2);
    }

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_Pixel_Sprite::Spawn_Pooling(void *pArg)
{
    if (FAILED(CGameObject::Spawn_Pooling()))
        return E_FAIL;

    if (SpriteParticleOptions *pOption = reinterpret_cast<SpriteParticleOptions *>(pArg))
    {
        ::memcpy(&m_tOption, &(pOption->tEffectOption), sizeof(EffectOptions));

        if (FAILED(Set_TextureInit(pOption->eType)))
        {
            MSG_BOX("CEffect_Pixel_Sprite::Spawn_Pooling, Type is invalid");
            return E_FAIL;
        }

        m_iTextureCount = m_pTextureCom->Get_Frame().m_iEndTex;

        // VB 준비
        SetOptions(m_tOption, false);
        // 생성과 동시에 발사
        Trigger();
    }

    return S_OK;
}

HRESULT CEffect_Pixel_Sprite::Despawn_Pooling()
{
    if (FAILED(CGameObject::Despawn_Pooling()))
        return E_FAIL;

    m_iAliveCount = 0;
    m_iTextureCount = 0;
    m_iVBCapacity = 0;
    m_tOption = {};
    m_vecParticles.clear();
    m_pTextureCom->Set_Zero_Frame();

    return S_OK;
}

HRESULT CEffect_Pixel_Sprite::Texture_Clone()
{
    CTexture::TEXINFO info{};
    struct AnimationDeffinition
    {
        const _tchar *tag;
        const _tchar *proto;
        int start;
        int end;
        float speed;
        bool loop;
    };
    AnimationDeffinition ttt[] =
    {
        { L"DeadBody",	L"Proto_Effect_DeadBody",   0,	16,	1.f,	false },
        {L"Glass",      L"Proto_Effect_Glass",      0,   3, 1.f,    false },
        {L"Bottle",     L"Proto_Effect_Bottle",     0,   3, 1.f,    false },
    };

    for (AnimationDeffinition &Element : ttt)
    {
        ::ZeroMemory(&info, sizeof(info));
        info.m_iStart = Element.start;
        info.m_iEndTex = Element.end;
        info.m_fSpeed = Element.speed;
        info.m_bLoop = Element.loop;

        if (FAILED(Add_Components(
            Element.tag,
            SCENE_STATIC,
            Element.proto,
            (CComponent **)&m_pTextureCom, &info)))
            return E_FAIL;
    }
   

    return S_OK;
}

void CEffect_Pixel_Sprite::SetUp_BillBoard()
{
    _matrix matView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, nullptr, &matView);

    _vec3 vRight = *(_vec3 *)&matView.m[0][0];
    _vec3 vUp = *(_vec3 *)&matView.m[1][0];
    _vec3 vLook = *(_vec3 *)&matView.m[2][0];

    m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
    m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
    m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}

void CEffect_Pixel_Sprite::SetOptions(const EffectOptions &tOption, _bool bRemakeVB)
{
    m_tOption = tOption;
    m_vecParticles.assign(m_iTextureCount, ParticleInfo{}); // 파티클 컨테이너 크기 = iPixelCount
    m_iAliveCount = 0;

    if (!bRemakeVB)
        return;

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
    
    _uint iMaxSprites{ 0 };
    iMaxSprites = m_iTextureCount;
    const int iVtxCount = iMaxSprites * 4;
    const int iTriCount = iMaxSprites * 2;

    // VB
    if (FAILED(m_pGraphicDev->CreateVertexBuffer(
        sizeof(VTXTEX) * iVtxCount,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        FVF_TEX, D3DPOOL_DEFAULT, &m_pVB, nullptr)))
    {
        MSG_BOX("VB create failed");
        return;
    }

    m_iVBCapacity = iVtxCount;


    if (FAILED(m_pGraphicDev->CreateIndexBuffer(
        sizeof(INDEX16) * iTriCount,
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, nullptr)))
    {
        MSG_BOX("IB create failed");
        return;
    }

    INDEX16 *pIndex = nullptr;
    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    pIndex[0]._0 = 0;
    pIndex[0]._1 = 1;
    pIndex[0]._2 = 2;    
    pIndex[1]._0 = 0;
    pIndex[1]._1 = 2;
    pIndex[1]._2 = 3;

    m_pIB->Unlock();
}

void CEffect_Pixel_Sprite::Trigger()
{
    Do_Once();
}

void CEffect_Pixel_Sprite::Do_Once()
{
    const int iIndex = (std::min)(m_iTextureCount, (_int)(m_vecParticles.size() - m_iAliveCount));
    for (int i = 0; i < iIndex; ++i)
    {
        ParticleInfo &tInfo = m_vecParticles[i];

        if (tInfo.bAlive)
            continue;

        tInfo.vPosition = _vec3{ 0.f, 0.f, 0.f };
        // 방향/속도
        _vec3 vDir = randomDir_HalfSphere();
        _float fSpeed = randRange(m_tOption.fSpeed_Min, m_tOption.fSpeed_Max);
        tInfo.vVelocity = vDir * fSpeed;

        _uint width;
        _uint height;
        m_pTextureCom->GetFrameSize(i, width, height);

        tInfo.fDurationTime = 0.f;
        tInfo.fLifeTime = m_tOption.fLife_Min;
        tInfo.fSize = (_float)width / 1024;
        tInfo.colorStart = m_tOption.colorStart;
        tInfo.colorEnd = m_tOption.colorEnd;
        tInfo.bAlive = true;
        tInfo.fAngle = randRange(-3.14159f, 3.14159f);
        tInfo.fRotateVel = randRange(-10.f, 10.f);
        ++m_iAliveCount;
    }
}

void CEffect_Pixel_Sprite::Particle_Update(_float fDeltaTime)
{
    if (m_iAliveCount <= 0)
        return;

    for (ParticleInfo &tElement : m_vecParticles)
    {
        if (!tElement.bAlive)
            continue;

        const float fNoiseFrequency = m_tOption.fNoiseFreq * 6.28f;

        // 가속 구성
        _vec3 fAccelVelocity{ 0,0,0 };
        switch (m_tOption.eMode)
        {
        case EffectMode::LINEAR:
        {
            fAccelVelocity += (-m_tOption.fDrag) * tElement.vVelocity;
        } break;
        case EffectMode::GRAVITY_ARC:
        {
            fAccelVelocity += _vec3{ 0, -9.8f, 0 };
            fAccelVelocity += (-m_tOption.fDrag) * tElement.vVelocity;
        } break;
        case EffectMode::CURVE_NOISE:
        {
            _vec3 vSrc = tElement.vVelocity;
            _float fLength = ::D3DXVec3Length(&vSrc);
            if (fLength > 1e-4f /*Epsilon*/)
            {
                vSrc /= fLength;
                _vec3 vWorldUp{ 0, 1.f, 0 };
                _vec3 vCrossResult;
                ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);
                if (::D3DXVec3LengthSq(&vCrossResult) < 1e-6f)
                {
                    ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);
                }
                ::D3DXVec3Normalize(&vCrossResult, &vCrossResult);
                float fSin = sinf(tElement.fPhase + fNoiseFrequency * tElement.fDurationTime);
                fAccelVelocity += vCrossResult * (m_tOption.fNoiseStrength * fSin);
            }
            fAccelVelocity += (-m_tOption.fDrag) * tElement.vVelocity;
        } break;
        case EffectMode::SPIRAL:
        {
            _vec3 vSrc = tElement.vVelocity;
            _float fLength = ::D3DXVec3Length(&vSrc);
            if (fLength > 1e-4f)
            {
                vSrc /= fLength;
                _vec3 vWorldUp{ 0, 1.f, 0 };
                _vec3 vCrossResult;
                ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);
                if (::D3DXVec3LengthSq(&vCrossResult) < 1e-6f)
                {
                    ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);
                }
                ::D3DXVec3Normalize(&vCrossResult, &vCrossResult);
                _float fSin = sinf(tElement.fPhase + fNoiseFrequency * tElement.fDurationTime);
                _float fCos = cosf(tElement.fPhase + fNoiseFrequency * tElement.fDurationTime);
                fAccelVelocity += (vCrossResult * fSin + vWorldUp * fCos) * m_tOption.fSpiralAmp;
            }
            fAccelVelocity += (-m_tOption.fDrag) * tElement.vVelocity;
        } break;
        }

        tElement.vVelocity += fAccelVelocity * fDeltaTime;
        tElement.vPosition += tElement.vVelocity * fDeltaTime;
        tElement.fAngle += tElement.fRotateVel * fDeltaTime;

        tElement.fDurationTime += fDeltaTime;
        if (tElement.fDurationTime >= tElement.fLifeTime)
        {
            tElement.bAlive = false;
            --m_iAliveCount;
        }
    }
}

HRESULT CEffect_Pixel_Sprite::Ready_VB()
{
    if (!m_pVB || m_iAliveCount <= 0) return E_FAIL;

    VTXTEX *vtx = nullptr;
    m_pVB->Lock(0, 0, (void **)&vtx, D3DLOCK_DISCARD);

    int iIndex{ 0 };
    for (ParticleInfo &tElement : m_vecParticles)
    {
        if (!tElement.bAlive)
            continue;

        _float fHalf = tElement.fSize * 0.5f;
        _float fCos = cosf(tElement.fAngle);
        _float fSin = sinf(tElement.fAngle);

        _vec3 vLocalOffset_0{ -fHalf, fHalf, 0.f };
        _vec3 vLocalOffset_1{ fHalf, fHalf, 0.f };
        _vec3 vLocalOffset_2{ fHalf, -fHalf, 0.f };
        _vec3 vLocalOffset_3{ -fHalf, -fHalf, 0.f };

        auto GetRotate = [&](const _vec3 &v)->_vec3
        { return _vec3(v.x * fCos - v.y * fSin, v.x * fSin + v.y * fCos, 0.f); };

        vtx[iIndex].vPosition = tElement.vPosition + GetRotate(vLocalOffset_0);
        vtx[iIndex].vTexUV = { 0.f, 0.f };
        ++iIndex;
        vtx[iIndex].vPosition = tElement.vPosition + GetRotate(vLocalOffset_1);
        vtx[iIndex].vTexUV = { 1.f, 0.f };
        ++iIndex;
        vtx[iIndex].vPosition = tElement.vPosition + GetRotate(vLocalOffset_2);
        vtx[iIndex].vTexUV = { 1.f, 1.f };
        ++iIndex;
        vtx[iIndex].vPosition = tElement.vPosition + GetRotate(vLocalOffset_3);
        vtx[iIndex].vTexUV = { 0.f, 1.f };
        ++iIndex;
    }

    m_pVB->Unlock();

	return S_OK;
}

_float CEffect_Pixel_Sprite::rand01() const
{
    return (_float)(rand() & 0x7fff) / 32767.f;
}

_float CEffect_Pixel_Sprite::randRange(_float a, _float b) const
{
    return a + (b - a) * rand01();
}

_vec3 CEffect_Pixel_Sprite::randomDir_HalfSphere() const
{
    float u = rand01() * 2.f - 1.f;      // cos theta in [-1,1]
    float phi = rand01() * 6.28f;
    float r = sqrtf((std::max)(0.f, 1.f - u * u));
    D3DXVECTOR3 d(r * cosf(phi), fabsf(u), r * sinf(phi)); // y>=0
    D3DXVec3Normalize(&d, &d);
    return d;
}

void CEffect_Pixel_Sprite::Effect_SetRenderState()
{
    switch (m_tOption.eBlendmode)
    {
    case BlendMode::ADDITIVE:
    {
        m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    } break;
    case BlendMode::ALPHA:
    {
        m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    } break;
    }
}

HRESULT CEffect_Pixel_Sprite::Set_TextureInit(SpriteParticleType _e)
{
    switch (_e)
    {
    case Engine::SpriteParticleType::DEADBODY:
        return Change_Texture(L"DeadBody");
    case Engine::SpriteParticleType::GLASS:
        return Change_Texture(L"Glass");
    case Engine::SpriteParticleType::BOTTLE:
        return Change_Texture(L"Bottle");
    }

    return E_FAIL;
}

HRESULT CEffect_Pixel_Sprite::Change_Texture(const _tchar *LayerTag)
{
    if (FAILED(CGameObject::Change_Component(LayerTag, (CComponent **)&m_pTextureCom)))
        return E_FAIL;

    if (m_pTextureCom)
        m_pTextureCom->Set_Zero_Frame();

    return S_OK;
}