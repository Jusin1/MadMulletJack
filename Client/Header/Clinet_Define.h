#pragma once

enum STATE {STATE_BEGIN, STATE_ON, STATE_END}; // state가 끝났는지 안 끝났는지 알기 위해

enum PLAYERSTATE {
	IDLE, JUMP, DASH_ATTACK, DASH, SLIED, KICK, ATTACK,
	ATTACK_INSTANT, ZOOMING, ZOOM, RELOAD, DOPING, WALL, OPENING, PLAYERDEAD, PLAYER_END};

enum WEAPON { WP_NON, WP_PISTOL, WP_SHOTGUN, WP_RIFLE, WP_KATANA, WP_SNIPER, WP_END };

enum WEAPON2 { WP_KICK, WP_KNIFE, WP_BOOK, WP2_END };

enum KILL { NORMAL_KILL, DASH_KILL, ENV_ELECT_KILL, ENV_WALL_KILL, ENV_EXPLORE_KILL, HEAD_KILL, BODY_KILL, BALL_KILL, KATANA_KILL, ICE_KILL };

enum UIMOVE { MV_NON, MV_RIGHT,MV_LEFT,MV_RL, 
              MV_UP,MV_DOWN, MV_UpDown,
              MV_ROTATIONZ, MV_END };

enum UISTATE {UI_CREATE,UI_ACTIVE,UI_NOACTIVE, UI_RENDERON, UI_RENDEROFF, UI_STATEEND ,UI_END }; // 굳이..?

struct PlayerStateInfo {
    // 플레이어의 state들을 가지고 있는 구조체. -> 이 info로 상태 좌지우지
	PLAYERSTATE ePlayerState;
	WEAPON		eWeapon;
	WEAPON2		eWeapon2;


    bool operator!=(const PlayerStateInfo& other) const {
        return (ePlayerState != other.ePlayerState ||
            eWeapon != other.eWeapon ||
            eWeapon2 != other.eWeapon2);
    }

    bool operator==(const PlayerStateInfo& other) const {
        return (ePlayerState == other.ePlayerState &&
            eWeapon == other.eWeapon &&
            eWeapon2 == other.eWeapon2);
    }
};

struct UIMoveInfo
{
    UIMOVE eUIMove; // 움직임 enum 값 -> 여러 움직임을 주고 싶으면 vector로 해둘까??
    bool  bStop;   // range 까지 움직이고 stop 할건지 말건지
    float fRange;  // fRange 움직임 범위
    float fSumRange; // 움직임 누적 범위
    
    bool IsRangeEnd() { return (bStop && (fRange <= fSumRange)); };
};