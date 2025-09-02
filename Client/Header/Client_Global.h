#pragma once
#include <random>

typedef struct tagEffectOptions EffectOptions;

extern bool g_ColiderRender;

extern mt19937_64 g_Rng;
extern const float g_Epsilon;

inline extern float Rand_Float(float fA, float fB);
inline extern int Rand_Int(int iA, int iB);
inline extern float Lerp_Float(float fA, float fB, float fT);
inline extern D3DXVECTOR3 Lerp_Vec3(D3DXVECTOR3 vA, D3DXVECTOR3 vB, float fT);
inline extern float Lenght_XZ(const D3DXVECTOR3 &v);
inline extern D3DXVECTOR3 Norm_XZ(const D3DXVECTOR3 &v);
inline extern D3DXVECTOR3 Normalize_Safe(const D3DXVECTOR3 &v, const D3DXVECTOR3 &fallback = D3DXVECTOR3(0, 0, 1));

extern EffectOptions Get_Preset_BulletSpark();
extern EffectOptions Get_Preset_Electric();
extern EffectOptions Get_Preset_Blood();

