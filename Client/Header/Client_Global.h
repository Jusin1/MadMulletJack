#pragma once

extern bool g_ColiderRender;
bool GetTextureWH(IDirect3DBaseTexture9* pBase, UINT& outW, UINT& outH);

typedef struct tagEffectOptions EffectOptions;

extern EffectOptions Get_Preset_BulletSpark();
extern EffectOptions Get_Preset_Electric();
extern EffectOptions Get_Preset_Blood();