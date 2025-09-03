#include "pch.h"
#include "Engine_Define.h"
#include "Client_Global.h"

std::mt19937_64 g_Rng{ 0xDEADBEEFCAFEBABEULL };
_bool g_ColiderRender = true;
constexpr float g_Epsilon = 1e-6f;

inline float Rand_Float(float fA, float fB)
{
    return std::uniform_real_distribution<float>(fA, fB)(g_Rng);
}

inline int Rand_Int(int iA, int iB)
{
    return std::uniform_int_distribution<int>(iA, iB)(g_Rng);
}

inline float Lerp_Float(float fA, float fB, float fT)
{
    return fA + (fB - fA) * fT;
}

inline D3DXVECTOR3 Lerp_Vec3(D3DXVECTOR3 vA, D3DXVECTOR3 vB, float fT)
{
    return vA + (vB - vA) * fT;
}

inline D3DXCOLOR Lerp_Color(const D3DXCOLOR &colorA, const D3DXCOLOR &colorB, float fT)
{
    D3DXCOLOR colorNew;
    
    colorNew.r = colorA.r + (colorB.r - colorA.r) * fT;
    colorNew.g = colorA.g + (colorB.g - colorA.g) * fT;
    colorNew.b = colorA.b + (colorB.b - colorA.b) * fT;
    colorNew.a = colorA.a;

    return colorNew;
}

inline float Lenght_XZ(const D3DXVECTOR3 &v)
{
    return std::sqrtf(v.x * v.x + v.z * v.z);
}

inline D3DXVECTOR3 Norm_XZ(const D3DXVECTOR3 &v)
{
    float fLength = Lenght_XZ(v);
    if (fLength < g_Epsilon)
        return D3DXVECTOR3(0, 0, 0);
    return D3DXVECTOR3(v.x / fLength, 0.f, v.z / fLength);

}

inline D3DXVECTOR3 Normalize_Safe(const D3DXVECTOR3 &v, const D3DXVECTOR3 &fallback)
{
    float fLength = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (fLength < g_Epsilon)
        return fallback;
    return _vec3(v.x / fLength, v.y / fLength, v.z / fLength);
}


EffectOptions Get_Preset_BulletSpark()
{
    EffectOptions o;
    o.eMode = EffectMode::LINEAR;
    o.iPixelCount = 24;
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

EffectOptions Get_Preset_Electric()
{
    EffectOptions o;
    o.eMode = EffectMode::SPIRAL;
    o.iPixelCount = 24;
    o.fLife_Min = 0.3f;
    o.fLife_Max = 0.5f;
    o.fSize_Min = 1.f;
    o.fSize_Max = 4.f;
    o.fSpeed_Min = 2.f;
    o.fSpeed_Max = 4.f;
    o.colorStart = D3DXCOLOR(0.5f, 0.8f, 1.0f, 1.0f);
    o.colorEnd = D3DXCOLOR(0.1f, 0.3f, 1.0f, 0.7f);
    o.eBlendmode = BlendMode::ADDITIVE;
    o.fSpiralAmp = 100.f;
    o.fSpiralFreq = 30.f;
    o.fDrag = 5.0f;
    return o;
}

EffectOptions Get_Preset_Blood()
{
    EffectOptions o;
    o.eMode = EffectMode::GRAVITY_ARC;
    o.iPixelCount = 60;
    o.fLife_Min = 0.5f;
    o.fLife_Max = 1.3f;
    o.fSize_Min = 3.f;
    o.fSize_Max = 8.f;
    o.fSpeed_Min = .7f;
    o.fSpeed_Max = 2.5f;
    o.colorStart = D3DXCOLOR(0.7f, 0.05f, 0.08f, 1.f);
    o.colorEnd = D3DXCOLOR(0.3f, 0.05f, 0.08f, 1.f);
    o.eBlendmode = BlendMode::ADDITIVE;
    o.fDrag = 2.f;
    return o;
}