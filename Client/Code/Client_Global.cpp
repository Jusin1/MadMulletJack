#include "pch.h"
#include "Engine_Define.h"
#include "Client_Global.h"

_bool g_ColiderRender = true;

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
    o.fSpiralAmp = 3.f;
    o.fSpiralFreq = 50.f;
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
    o.fSize_Min = 1.f;
    o.fSize_Max = 5.f;
    o.fSpeed_Min = .7f;
    o.fSpeed_Max = 2.5f;
    o.colorStart = D3DXCOLOR(0.7f, 0.05f, 0.08f, 1.f);
    o.colorEnd = D3DXCOLOR(0.3f, 0.05f, 0.08f, 1.f);
    o.eBlendmode = BlendMode::ALPHA;
    o.fDrag = 2.f;
    return o;
}