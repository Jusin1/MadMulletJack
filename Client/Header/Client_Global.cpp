#include "pch.h"
#include "Engine_Define.h"
#include "Client_Global.h"

EffectOptions Get_Preset_BulletSpark()
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

EffectOptions Get_Preset_Electric()
{
    EffectOptions o;
    o.eMode = EffectMode::GRAVITY_ARC;
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
    o.fDrag = 1.0f;
    return o;
}

EffectOptions Get_Preset_Blood()
{
    EffectOptions o;
    o.eMode = EffectMode::GRAVITY_ARC;
    o.iPixelCount = 60;
    o.iRepeatCount = 2;
    o.fRepeatTime = 0.5f;
    o.fLife_Min = 0.5f;
    o.fLife_Max = 1.3f;
    o.fSize_Min = 2.f;
    o.fSize_Max = 10.f;
    o.fSpeed_Min = .7f;
    o.fSpeed_Max = 3.f;
    o.colorStart = D3DXCOLOR(0.65f, 0.05f, 0.08f, 1.f);
    o.colorEnd = D3DXCOLOR(0.65f, 0.05f, 0.08f, 1.f);
    o.eBlendmode = BlendMode::ADDITIVE;
    o.fDrag = 0.6f;
    return o;
}