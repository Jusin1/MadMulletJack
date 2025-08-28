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
        MSG_BOX("CMonster_Suit Clone Failed");
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

    if (pArg)
    {
        m_tOption = *reinterpret_cast<EffectOptions *>(pArg);
        // VB 준비
        SetOptions(m_tOption, /*reallocateVB*/true);
        // 생성과 동시에 발사 원하면:
        Trigger();
    }
    
    return S_OK;
}

_int CEffect_Pixel::Update_GameObject(const _float &fTimeDelta)
{
    if (!m_bActive && m_iAliveCount <= 0)
        return DEAD;

    // 반복 방출 스케줄
    if (m_bActive && m_iRepeatCounting < (std::max)(1, m_tOption.iRepeatCount))
    {
        m_fRepeatTimer += fTimeDelta;
        if (m_iRepeatCounting == 0)
        {
            ++m_iRepeatCounting;
        }
        else if (m_fRepeatTimer >= m_tOption.fRepeatTime)
        {
            m_fRepeatTimer = 0.f;
            Do_Once();
            ++m_iRepeatCounting;

            if (m_iRepeatCounting >= m_tOption.iRepeatCount)
            {
                m_bActive = false;
            }
        }
    }

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
    Particle_Update(fTimeDelta);
    return NO_EVENT;
}

void CEffect_Pixel::Render_GameObject()
{
    if (m_iAliveCount <= 0 || !m_pVB) return;

    // 렌더상태
    m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPIXELCOLOR));

    m_pGraphicDev->SetFVF(FVF_PIXEL_COLOR);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    
    Effect_SetRenderState();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // 포인트 스프라이트 모드
    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD *)&(const float &)10.0f)); // 하드웨어 최대치 힌트
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    Ready_VB();
    m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, 0, m_iAliveCount);

    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void CEffect_Pixel::SetOptions(const EffectOptions &tOption, _bool bRemakeVB)
{
    m_tOption = tOption;

    // 파티클 컨테이너(+여유)
    const int iCapacity = (std::max)(tOption.fMax_ParticlesBufferSize, tOption.iPixelCount * (std::max)(1, tOption.iRepeatCount) * 2);
    m_vecParticles.clear();
    m_vecParticles.resize(iCapacity);
    m_iAliveCount = 0;

    if (bRemakeVB)
    {
        if (m_pVB)
            Safe_Release(m_pVB);

        m_iVBCapacity = iCapacity;

        if (FAILED(m_pGraphicDev->CreateVertexBuffer(
            sizeof(VTXPIXELCOLOR) * m_iVBCapacity,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            FVF_PIXEL_COLOR,
            D3DPOOL_DEFAULT,
            &m_pVB,
            nullptr)))
        {
            m_iVBCapacity = 0;
        }
    }
}

void CEffect_Pixel::Trigger()
{
    m_iRepeatCounting = 0;
    m_fRepeatTimer = 0.f;
    m_bActive = true;
    Do_Once();
}

EffectOptions CEffect_Pixel::Get_Preset_BulletSpark()
{
    EffectOptions o;
    o.eMode = EffectMode::LINEAR;
    o.iPixelCount = 24;
    o.iRepeatCount = 1;
    o.fLife_Min = 0.5f;
    o.fLife_Max = 1.5f;
    o.fSize_Min = 6.f;  
    o.fSize_Max = 10.f;
    o.fSpeed_Min = 1.8f; 
    o.fSpeed_Max = 3.f;
    o.colorStart = D3DXCOLOR(1.0f, 0.9f, 0.6f, 1.0f);
    o.colorEnd = D3DXCOLOR(1.0f, 0.4f, 0.1f, 0.0f);
    o.eBlendmode = BlendMode::ADDITIVE;
    o.fDrag = 2.0f;
    return o;
}

EffectOptions CEffect_Pixel::Get_Preset_Electric()
{
    EffectOptions o;
    o.eMode = EffectMode::CURVE_NOISE;
    o.iPixelCount = 32;
    o.iRepeatCount = 2;
    o.fRepeatTime = 0.03f;
    o.fLife_Min = 0.5f;
    o.fLife_Max = 1.5f;
    o.fSize_Min = 2.f;
    o.fSize_Max = 5.f;
    o.fSpeed_Min = 1.f;
    o.fSpeed_Max = 1.8f;
    o.colorStart = D3DXCOLOR(0.5f, 0.8f, 1.0f, 1.0f);
    o.colorEnd = D3DXCOLOR(0.2f, 0.5f, 1.0f, 0.0f);
    o.eBlendmode = BlendMode::ADDITIVE;
    o.fNoiseStrength = 40.f;
    o.fNoiseFrequency = 35.f;
    o.fDrag = 1.0f;
    return o;
}

EffectOptions CEffect_Pixel::Get_Preset_Blood()
{
    EffectOptions o;
    o.eMode = EffectMode::GRAVITY_ARC;
    o.iPixelCount = 60;
    o.iRepeatCount = 1;
    o.fLife_Min = 0.5f;
    o.fLife_Max = 1.3f;
    o.fSize_Min = 0.1f;
    o.fSize_Max = 0.2f;
    o.fSpeed_Min = .7f;
    o.fSpeed_Max = 3.f;
    // 검붉은 톤
    o.colorStart = D3DXCOLOR(0.65f, 0.05f, 0.08f, 0.95f);
    o.colorEnd = D3DXCOLOR(0.65f, 0.05f, 0.08f, 0.0f);
    o.eBlendmode = BlendMode::ALPHA;
    o.fDrag = 0.6f;
    return o;
}

void CEffect_Pixel::Do_Once()
{
    const int iCount = (std::min)(m_tOption.iPixelCount, (_int)(m_vecParticles.size() - m_iAliveCount));
    for (int i = 0; i < iCount; ++i)
    {
        // 빈 슬롯 찾기
        _int iSlot{ -1 };
        for (int j = 0; j < m_vecParticles.size(); ++j)
        {
            if(!m_vecParticles[j].bAlive)
            {
                iSlot = j;
                break;
            }
        }

        // 빈 슬롯 없으면 나가기
        if (iSlot < 0)
            break;

        ParticleInfo &tInfo = m_vecParticles[iSlot];
        tInfo.vPosition = m_vTargetPosition;

        // 방향/속도
        _vec3 vDir = randomDir_HalfSphere();
        _float fSpeed = randRange(m_tOption.fSpeed_Min, m_tOption.fSpeed_Max);
        tInfo.vVelocity = vDir * fSpeed;

        tInfo.fDurationTime = 0.f;
        tInfo.fLifeTime = randRange(m_tOption.fLife_Min, m_tOption.fLife_Max);
        tInfo.fSize = randRange(m_tOption.fSize_Min, m_tOption.fSize_Max);
        tInfo.colorStart = ToColor(m_tOption.colorStart);
        tInfo.colorEnd = ToColor(m_tOption.colorEnd);
        tInfo.fPhase = rand01() * 3.14f;
        tInfo.bAlive = true;
        ++m_iAliveCount;
    }
}

void CEffect_Pixel::Particle_Update(_float fDeltaTime)
{
    // 풀링에서 회수처리
    if (m_iAliveCount <= 0) return;

    const float fNoiseFrequency = m_tOption.fNoiseFrequency * 3.14f;

    for (ParticleInfo &tElement : m_vecParticles)
    {
        if (!tElement.bAlive)
            continue;

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
                // 월드업과 외적
                vSrc /= fLength;
                _vec3 vWorldUp{ 0, 1.f, 0 };
                _vec3 vCrossResult;
                ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);

                if (::D3DXVec3LengthSq(&vCrossResult) < 1e-6f)
                {
                    vWorldUp = { 0, 1.f, 0 };
                    ::D3DXVec3Cross(&vCrossResult, &vSrc, &vWorldUp);
                }

                ::D3DXVec3Normalize(&vCrossResult, &vCrossResult);

                _float fSin = sinf(tElement.fPhase + fNoiseFrequency * tElement.fDurationTime);
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
                    vWorldUp = { 0, 1.f, 0 };
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
	if (!m_pVB || m_iAliveCount <= 0) return E_FAIL;

	VTXPIXELCOLOR *vtx = nullptr;
	m_pVB->Lock(0, sizeof(VTXPIXELCOLOR) * m_iVBCapacity, (void **)&vtx, D3DLOCK_DISCARD);

	int iCount{ 0 };
	for (ParticleInfo &tElement : m_vecParticles)
	{
		if (!tElement.bAlive)
			continue;

		// 경과 시간 / 생명 시간으로 색깔 보간!
		float fSrc = (std::min)(1.f, (std::max)(0.f, tElement.fDurationTime / tElement.fLifeTime));
		vtx[iCount].vPosition = tElement.vPosition;
		vtx[iCount].dwColor = ToColor(LerpColor(tElement.colorStart, tElement.colorEnd, fSrc));
		vtx[iCount].fSize = tElement.fSize;
		++iCount;
	}
	m_pVB->Unlock();
}

D3DXCOLOR CEffect_Pixel::LerpColor(const D3DXCOLOR &a, const D3DXCOLOR &b, _float _f)
{
	return a + (b - a) * _f;
}

float CEffect_Pixel::rand01() const
{
	return (_float)(rand() & 0x7fff) / 32767.f;
}

float CEffect_Pixel::randRange(float a, float b) const
{
	return a + (b-a) * rand01();
}

D3DXVECTOR3 CEffect_Pixel::randomDir_HalfSphere() const
{
	float u = rand01() * 2.f - 1.f;      // cos theta in [-1,1]
	float phi = rand01() * 3.14f;
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
