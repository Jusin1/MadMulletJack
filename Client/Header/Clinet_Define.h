#pragma once


enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_LOGO, SCENE_STAGE, SCENE_END };

enum PLAYERSTATE {
	IDLE, JUMP, DASH_ATTACK, DASH, SLIED, KICK, ATTACK,
	ATTACK_INSTANT, RELOAD, HIT, DOPING, WALL, OPENING, PLAYERDEAD, PLAYER_END};

enum WEAPON { WP_NON, WP_PISTOL, WP_SHOTGUN, WP_RIFLE, WP_KATANA, WP_SNIPER, WP_END };

enum WEAPON2 { WP_KICK, WP_KNIFE, WP_BOOK, WP2_END };

enum KILL { NORMAL_KILL, DASH_KILL, ENV_ELECT_KILL, ENV_WALL_KILL, ENV_EXPLORE_KILL, HEAD_KILL, BODY_KILL, BALL_KILL, KATANA_KILL, ICE_KILL };

enum UIMOVE { MV_NON, MV_RIGHT,MV_LEFT,MV_RL, 
              MV_UP,MV_DOWN, MV_UpDown,
              MV_ROTATIONZ, MV_END };

struct PlayerStateInfo {

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