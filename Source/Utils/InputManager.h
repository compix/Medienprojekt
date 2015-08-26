#pragma once
#include <map>
#include <entityx/entityx.h>
#include "../Events/MenuShowEvent.h"
#include "../Events/ResetGameEvent.h"
#include <SFML/Window/Joystick.hpp>
#include "../GameConstants.h"

namespace sf
{
	class Event;
}

using entityx::Receiver;

enum PlayerButton
{
	BOMB,
	SKILL,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	COUNT
};

struct PlayerInput
{
	float moveX;
	float moveY;
	bool buttonPressed[PlayerButton::COUNT];
};

struct KeycodeMapEntry
{
	PlayerButton button;
	int playerIndex;
};

struct JoystickConfig
{
	std::string name;
	bool configured = false;
	int skillButton;
	int bombButton;
	int playerIndex;
	sf::Joystick::Axis xAxis;
	sf::Joystick::Axis yAxis;
	float scaleX;
	float scaleY;
};

class InputManager : public Receiver<InputManager>
{
public:
	InputManager();
	~InputManager();

	PlayerInput &getPlayerInput(int id);

	static const char *getKeyName(int code);
	static int getKeyCode(const char *name);
	static sf::Joystick::Axis getAxis(const char *name);

	void receive(const sf::Event &evt);
	void receive(const MenuShowEvent &evt);
	void receive(const ResetGameEvent &evt);
	
	void update();

private:
	PlayerInput m_playerInputs[GameConstants::MAX_PLAYERS];
	std::map<int, KeycodeMapEntry> m_keycodeMap;
	JoystickConfig m_storedJoystickConfigs[GameConstants::MAX_PLAYERS];
	JoystickConfig *m_joystickMap[sf::Joystick::Count];

	void reset();
	bool loadConfigFromJson(const std::string& path);
	void bindKey(int playerIndex, PlayerButton button, int keyCode);

	void onKeyPressed(int code);
	void onKeyReleased(int code);
	void updatePlayerMove(PlayerInput &playerInput);

	void onJoystickButtonPressed(int id, int button);
	void onJoystickButtonReleased(int id, int button);
	void onJoystickConnected(int id);
	void onJoystickDisconnected(int id);
};

