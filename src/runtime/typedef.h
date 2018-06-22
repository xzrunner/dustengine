#pragma once

namespace rt
{

enum KeyType
{
	KEY_UNKNOWN = 0,

	KEY_A = 65,
	KEY_B = 66,
	KEY_C = 67,
	KEY_D = 68,
	KEY_E = 69,
	KEY_F = 70,
	KEY_G = 71,
	KEY_H = 72,
	KEY_I = 73,
	KEY_J = 74,
	KEY_K = 75,
	KEY_L = 76,
	KEY_M = 77,
	KEY_N = 78,
	KEY_O = 79,
	KEY_P = 80,
	KEY_Q = 81,
	KEY_R = 82,
	KEY_S = 83,
	KEY_T = 84,
	KEY_U = 85,
	KEY_V = 86,
	KEY_W = 87,
	KEY_X = 88,
	KEY_Y = 89,
	KEY_Z = 90,

	KEY_RIGHT = 262,
	KEY_LEFT  = 263,
	KEY_DOWN  = 264,
	KEY_UP    = 265,

	KEY_F1  = 290,
	KEY_F2  = 291,
	KEY_F3  = 292,
	KEY_F4  = 293,
	KEY_F5  = 294,
	KEY_F6  = 295,
	KEY_F7  = 296,
	KEY_F8  = 297,
	KEY_F9  = 298,
	KEY_F10 = 299,
	KEY_F11 = 300,
	KEY_F12 = 301,
};

enum KeyMod
{
	KEY_MOD_SHIFT   = 0x0001,
	KEY_MOD_CONTROL = 0x0002,
	KEY_MOD_ALT     = 0x0004,
};

}