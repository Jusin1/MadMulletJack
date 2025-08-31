#pragma once

typedef struct tagEffectOptions EffectOptions;

extern bool g_ColiderRender;
bool GetTextureWH(IDirect3DBaseTexture9* pBase, UINT& outW, UINT& outH);

extern EffectOptions Get_Preset_BulletSpark();
extern EffectOptions Get_Preset_Electric();
extern EffectOptions Get_Preset_Blood();