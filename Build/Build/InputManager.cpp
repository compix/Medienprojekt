#include "InputManager.h"
#include <SFML/Window.hpp>
#include <assert.h>
#include <iostream>

using namespace std;

InputManager::InputManager()
{
	for (int i = 0; i < MAX_PLAYER_INPUTS; i++)
	{
		PlayerInput &pi = m_playerInputs[i];
		pi.moveX = 0;
		pi.moveY = 0;
		for (int j = 0; j < PlayerButton::COUNT; j++)
		{
			pi.buttonPressed[j] = false;
		}
	}

	// player 0 dummy binding, to be replaced by menu/config
	bindKey(0, PlayerButton::BOMB, sf::Keyboard::Key::Space);
	bindKey(0, PlayerButton::SKILL, sf::Keyboard::Key::LControl);
	bindKey(0, PlayerButton::UP, sf::Keyboard::Key::Up);
	bindKey(0, PlayerButton::DOWN, sf::Keyboard::Key::Down);
	bindKey(0, PlayerButton::LEFT, sf::Keyboard::Key::Left);
	bindKey(0, PlayerButton::RIGHT, sf::Keyboard::Key::Right);
}

InputManager::~InputManager()
{
}

PlayerInput &InputManager::getPlayerInput(int id)
{
	assert(id >= 0 && id < MAX_PLAYER_INPUTS);
	return m_playerInputs[id];
}

void InputManager::bindKey(int playerIndex, PlayerButton button, int keyCode)
{
	auto &entry = m_keycodeMap[keyCode];
	entry.button = button;
	entry.playerIndex = playerIndex;
}

void InputManager::handleEvent(const sf::Event &evt)
{
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		onKeyPressed(evt.key.code);
		break;
	case sf::Event::KeyReleased:
		onKeyReleased(evt.key.code);
		break;
	case sf::Event::JoystickButtonPressed:
		onJoystickButtonPressed(evt.joystickButton.joystickId, evt.joystickButton.button);
		break;
	case sf::Event::JoystickButtonReleased:
		onJoystickButtonReleased(evt.joystickButton.joystickId, evt.joystickButton.button);
		break;
	case sf::Event::JoystickConnected:
		onJoystickConnected(evt.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
		onJoystickDisconnected(evt.joystickConnect.joystickId);
		break;
	}
}


void InputManager::onKeyPressed(int code)
{
	auto it = m_keycodeMap.find(code);
	if (it != m_keycodeMap.end())
	{
		auto &player = m_playerInputs[it->second.playerIndex];
		player.buttonPressed[it->second.button] = true;
		switch (it->second.button)
		{
		case PlayerButton::UP:
		case PlayerButton::DOWN:
		case PlayerButton::LEFT:
		case PlayerButton::RIGHT:
			updatePlayerMove(player);
			break;
		}
	}
	cout << "Key pressed: " << getKeyName(code) << endl;
}

void InputManager::onKeyReleased(int code)
{
	auto it = m_keycodeMap.find(code);
	if (it != m_keycodeMap.end())
	{
		switch (it->second.button)
		{
		case PlayerButton::UP:
		case PlayerButton::DOWN:
		case PlayerButton::LEFT:
		case PlayerButton::RIGHT:
			auto &player = m_playerInputs[it->second.playerIndex];
			player.buttonPressed[it->second.button] = false;
			updatePlayerMove(player);
			break;
		}
	}
	cout << "Key released: " << getKeyName(code) << endl;
}

void InputManager::updatePlayerMove(PlayerInput &playerInput)
{
	bool up = playerInput.buttonPressed[PlayerButton::UP];
	bool down = playerInput.buttonPressed[PlayerButton::DOWN];
	bool left = playerInput.buttonPressed[PlayerButton::LEFT];
	bool right = playerInput.buttonPressed[PlayerButton::RIGHT];

	if (left != right)
		playerInput.moveX = left ? -1 : 1;
	else
		playerInput.moveX = 0;

	if (up != down)
		playerInput.moveY = up ? -1 : 1;
	else
		playerInput.moveY = 0;

	// Normalize
	if (playerInput.moveX && playerInput.moveY)
	{
		float len = sqrtf(playerInput.moveX * playerInput.moveX + playerInput.moveY * playerInput.moveY);
		playerInput.moveX /= len;
		playerInput.moveY /= len;
	}
	cout << playerInput.moveX << "/" << playerInput.moveY << endl;
}

void InputManager::onJoystickButtonPressed(int id, int button)
{
	cout << "Joystick button pressed: " << id << "/" << button << endl;
}

void InputManager::onJoystickButtonReleased(int id, int button)
{
	cout << "Joystick button released: " << id << "/" << button << endl;
}

void InputManager::onJoystickConnected(int id)
{
	sf::Joystick::Identification info = sf::Joystick::getIdentification(id);
	cout << "Joystick connected: " << info.name.toAnsiString() << endl;
}

void InputManager::onJoystickDisconnected(int id)
{
	sf::Joystick::Identification info = sf::Joystick::getIdentification(id);
	cout << "Joystick disconnected: " << info.name.toAnsiString() << endl;
}

// Keep in sync with sf::Keyboard::Key
static const char *keyNames[] =
{
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Escape",
	"left Control",
	"left Shift",
	"left Alt",
	"left Super",
	"right Control",
	"right Shift",
	"right Alt",
	"right Super",
	"Menu",
	"[",
	"]",
	",",
	",",
	".",
	"'",
	"/",
	"\\",
	"~",
	"=",
	"-",
	"Space",
	"Return",
	"Backspace",
	"Tab",
	"Page up",
	"Page down",
	"End",
	"Home",
	"Insert",
	"Delete",
	"+",
	"-",
	"*",
	"/",
	"Left arrow",
	"Right arrow",
	"Up arrow",
	"Down arrow",
	"numpad 0",
	"numpad 1",
	"numpad 2",
	"numpad 3",
	"numpad 4",
	"numpad 5",
	"numpad 6",
	"numpad 7",
	"numpad 8",
	"numpad 9",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"Pause"
};

static const int keyNamesLength = (sizeof(keyNames) / sizeof(*keyNames));
static_assert (keyNamesLength == sf::Keyboard::Key::KeyCount, "sf::Keyboard::Key::KeyCount does not match expectations");

const char *InputManager::getKeyName(int code)
{
	if (code >= 0 && code < sizeof(keyNames))
		return keyNames[code];
	return nullptr;
}

int InputManager::getKeyCode(const char *name) {
	if (name)
	{
		for (int i = 0; i < keyNamesLength; i++)
		{
			if (_strcmpi(keyNames[i], name) == 0)
				return i;
		}
	}
	return -1;
}
