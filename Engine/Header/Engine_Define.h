#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <array>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ctime>

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#define KEY_BUTTON_HOLD(key) CDInputMgr::GetInstance()->GetButton(key)
#define KEY_BUTTON_DOWN(key) CDInputMgr::GetInstance()->GetButtonDown(key)
#define KEY_BUTTON_UP(key) CDInputMgr::GetInstance()->GetButtonUp(key)

#define IS_LBUTTON_HOLD CDInputMgr::GetInstance()->GetMouseButton(0)
#define IS_RBUTTON_HOLD CDInputMgr::GetInstance()->GetMouseButton(1)
#define IS_MBUTTON_HOLD CDInputMgr::GetInstance()->GetMouseButton(2)

#define IS_LBUTTON_DOWN CDInputMgr::GetInstance()->GetMouseButtonDown(0)
#define IS_RBUTTON_DOWN CDInputMgr::GetInstance()->GetMouseButtonDown(1)
#define IS_MBUTTON_DOWN CDInputMgr::GetInstance()->GetMouseButtonDown(2)

#define IS_LBUTTON_UP CDInputMgr::GetInstance()->GetMouseButtonUp(0)
#define IS_RBUTTON_UP CDInputMgr::GetInstance()->GetMouseButtonUp(1)
#define IS_MBUTTON_UP CDInputMgr::GetInstance()->GetMouseButtonUp(2)


#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#define DEAD 1
#define NO_EVENT 0
#define NONE_HEIGHT		9999999.f
#endif

using namespace std;
using namespace Engine;

#endif // Engine_Define_h__
