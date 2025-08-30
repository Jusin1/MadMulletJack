#include "Engine_Define.h"
#include "CRenderer.h"
#include "CEffect_Pixel.h"

CEffect_Pixel::CEffect_Pixel(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEffect_Pixel::CEffect_Pixel(const CEffect_Pixel &rhs)
	: CGameObject(rhs)
{
}

CEffect_Pixel::~CEffect_Pixel()
{
	
}

void CEffect_Pixel::Free()
{
    Safe_Release(m_pVB);
	CGameObject::Free();
}

CEffect_Pixel *CEffect_Pixel::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Pixel *pInstance = new CEffect_Pixel(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CEffect_Pixel Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject *CEffect_Pixel::Clone(void *pArg)
{
    CEffect_Pixel *pInstance = new CEffect_Pixel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CEffect_Pixel Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CEffect_Pixel::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Pixel::Initialize(void *pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    SetOptions(EffectOptions{}, true);

    return S_OK;
}

_int CEffect_Pixel::Update_GameObject(const _float &fTimeDelta)
{
    if (m_iAliveCount <= 0)
        return DEAD;

    m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

    Particle_Update(fTimeDelta);
    return NO_EVENT;
}

void CEffect_Pixel::Render_GameObject()
{
    if (m_iAliveCount <= 0 || !m_pVB)
        return;

    if (FAILED(Ready_VB()))
    {
        MSG_BOX("CEffect_Pixel::Render_GameObject, Failed");
        return;
    }
    m_pTransformCom->Apply_WorldMatrix();

    // 렌더상태
    m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPIXELCOLOR));
    m_pGraphicDev->SetFVF(FVF_PIXEL_COLOR);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    Effect_SetRenderState();

    // 포인트 스프라이트 모드
    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    //m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.f));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.f));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.f));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(m_tOption.fSize_Min));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(m_tOption.fSize_Max));

    m_pGraphicDev->SetTexture(0, nullptr);
    //m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    //m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, 0, m_iAliveCount);

    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    //m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    //m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void CEffect_Pixel::SetOptions(const EffectOptions &tOption, _bool bRemakeVB)
{
    m_tOption = tOption;

    const int iCapacity = tOption.iPixelCount + 32;

    m_vecParticles.clear();
    m_vecParticles.resize(iCapacity);
    m_iAliveCount = 0;

    if (bRemakeVB)
    {
        if (m_pVB)
            Safe_Release(m_pVB);

        m_iVBCapacity = iCapacity;

        if (FAILED(m_pGraphicDev->CreateVertexBuffer(
            sizeof(VTXPIXELCOLOR) * 128, /* 여유치 */
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            FVF_PIXEL_COLOR,
            D3DPOOL_DEFAULT,
            &m_pVB,
            nullptr)))
        {
            MSG_BOX("CEffect_Pixel::SetOptions, Failed");
            m_iVBCapacity = 0;
        }
    }
}

void CEffect_Pixel::Trigger()
{
    Do_Once();
}

HRESULT CEffect_Pixel::Spawn_Pooling(void *pArg)
{
    if (FAILED(CGameObject::Spawn_Pooling()))
        return E_FAIL;

    if (EffectOptions *pOption = reinterpret_cast<EffectOptions *>(pArg))
    {
        ::memcpy(&m_tOption, pOption, sizeof(EffectOptions));
        // VB 준비
        SetOptions(m_tOption, false);
        // 생성과 동시에 발사
        Trigger();
    }

    return S_OK;
}

HRESULT CEffect_Pixel::Despawn_Pooling()
{
    if (FAILED(CGameObject::Despawn_Pooling()))
        return E_FAIL;

    m_iAliveCount = 0;
    m_iVBCapacity = 0;
    m_tOption = {};
    m_vecParticles.clear();

    return S_OK;
}

void CEffect_Pixel::Do_Once()
{
    const int iIndex = (std::min)(m_tOption.iPixelCount, (_int)(m_vecParticles.size() - m_iAliveCount));
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

        tInfo.fDurationTime = 0.f;
        tInfo.fLifeTime = randRange(m_tOption.fLife_Min, m_tOption.fLife_Max);
        tInfo.fSize = randRange(m_tOption.fSize_Min, m_tOption.fSize_Max);
        tInfo.colorStart = m_tOption.colorStart;
        tInfo.colorEnd = m_tOption.colorEnd;
        tInfo.bAlive = true;
        ++m_iAliveCount;
    }
}

void CEffect_Pixel::Particle_Update(_float fDeltaTime)
{
    // 풀링에서 회수처리
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

        tElement.fDurationTime += fDeltaTime;
        if (tElement.fDurationTime >= tElement.fLifeTime)
        {
            tElement.bAlive = false;
            --m_iAliveCount;
        }
    }
}

HRESULT CEffect_Pixel::Ready_VB()
{
	if (!m_pVB || m_iAliveCount <= 0)
        return E_FAIL;

	VTXPIXELCOLOR *vtx = nullptr;
	m_pVB->Lock(0, sizeof(VTXPIXELCOLOR) * m_iVBCapacity, (void **)&vtx, D3DLOCK_DISCARD);

	int iIndex{ 0 };
	for (ParticleInfo &tElement : m_vecParticles)
	{
		if (!tElement.bAlive)
			continue;

		// 경과 시간 / 생명 시간으로 색깔 보간!
		float fSrc = (std::min)(1.f, (std::max)(0.f, tElement.fDurationTime / tElement.fLifeTime));
		vtx[iIndex].vPosition = tElement.vPosition;
		vtx[iIndex].dwColor = ToColor(LerpColor(tElement.colorStart, tElement.colorEnd, fSrc));
		vtx[iIndex].fSize = tElement.fSize;
		++iIndex;
	}
	m_pVB->Unlock();
    return S_OK;
}

D3DXCOLOR CEffect_Pixel::LerpColor(const D3DXCOLOR &a, const D3DXCOLOR &b, _float _f)
{
	return a + (b - a) * _f;
}

float CEffect_Pixel::rand01() const
{
	return (_float)(rand() & 0x7fff) / 32767.f;
}

float CEffect_Pixel::randRange(_float a, _float b) const
{
	return a + (b-a) * rand01();
}

D3DXVECTOR3 CEffect_Pixel::randomDir_HalfSphere() const
{
	float u = rand01() * 2.f - 1.f;      // cos theta in [-1,1]
	float phi = rand01() * 6.28f;
	float r = sqrtf((std::max)(0.f, 1.f - u * u));
	D3DXVECTOR3 d(r * cosf(phi), fabsf(u), r * sinf(phi)); // y>=0
	D3DXVec3Normalize(&d, &d);
	return d;
}

void CEffect_Pixel::Effect_SetRenderState()
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
