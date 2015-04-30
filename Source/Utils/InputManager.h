#pragma once
#include <map>

namespace sf {
	class Event;
}

const int MAX_PLAYER_INPUTS = 4;

enum PlayerButton {
	BOMB,
	SKILL,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	COUNT
};

struct PlayerInput {
	float moveX;
	float moveY;
	bool buttonPressed[PlayerButton::COUNT];
};

struct KeycodeMapEntry {
	PlayerButton button;
	int playerIndex;
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	PlayerInput &getPlayerInput(int id);

	void handleEvent(const sf::Event &evt);

	static const char *getKeyName(int code);
	static int getKeyCode(const char *name);

private:
	PlayerInput m_playerInputs[MAX_PLAYER_INPUTS];
	std::map<int, KeycodeMapEntry> m_keycodeMap;
	std::map<int, int> m_joystickMap;

	void bindKey(int playerIndex, PlayerButton button, int keyCode);

	void onKeyPressed(int code);
	void onKeyReleased(int code);
	void updatePlayerMove(PlayerInput &playerInput);

	void onJoystickButtonPressed(int id, int button);
	void onJoystickButtonReleased(int id, int button);
	void onJoystickConnected(int id);
	void onJoystickDisconnected(int id);
};

