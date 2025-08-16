#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

#include <string>
using std::wstring;

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;		
		D3DCOLOR		dwColor;

	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec2		vTexUV;

	}VTXTEX;
	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1;


	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagVertexCubeColor
	{
		_vec3		vPosition;
		D3DCOLOR	dwColor;

	}VTXCUBECOLOR;

	const _ulong	FVF_CUBE_COLOR = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;

	
	///////////////////////////////////////////////////////////

	typedef struct tagGridPanelData
	{
		WallType eType = WallType::WALL_HOR;
		unsigned long dwCountX = 2;
		unsigned long dwCountY = 2;
		unsigned long dwCountZ = 2;
		unsigned long dwInterval = 1;
	} PANELDATA;

	typedef struct tagTransformData
	{
		float Right[3]{ 1.f, 0.f, 0.f };
		float Up[3]{ 0.f, 1.f, 0.f };
		float Look[3]{ 0.f, 0.f, 1.f };
		float Pos[3]{ 0.f, 0.f, 0.f };
	} TRANSFORMDATA;

	typedef struct tagTextureData
	{
		wstring OriginComponentName = L"";
	} TEXTUREDATA;

	typedef struct tagMapObjectData
	{
		ObjectCategory eCategory = ObjectCategory::NONE;
		_uint iType = 0;
		D3DCOLOR dwColor;
		TEXTUREDATA texture;
		TRANSFORMDATA transform;
		PANELDATA panelBuffer;
	} MAPOBJECTDATA;
}

#endif // Engine_Struct_h__
