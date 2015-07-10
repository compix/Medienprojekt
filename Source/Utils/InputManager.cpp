#include "InputManager.h"
#include <SFML/Window.hpp>
#include <assert.h>
#include <iostream>
#include <string.h>
#include "../GameGlobals.h"
#include "json/json.h"
#include <fstream>
#include "Exceptions.h"

using namespace std;


static const char *axisNames[] = {
	"X",
	"Y",
	"Z",
	"R",
	"U",
	"V",
	"PovX",
	"PovY"
};

InputManager::InputManager()
{
	GameGlobals::events->subscribe<MenuShowEvent>(*this);
	GameGlobals::events->subscribe<sf::Event>(*this);

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
	
	loadConfigFromJson("input.json");
	
	for(unsigned int i=0; i<sf::Joystick::Count; i++) {
		m_joystickMap[i] = nullptr;
		if(sf::Joystick::isConnected(i))
			onJoystickConnected(i);
	}
}

InputManager::~InputManager() { }

bool InputManager::loadConfigFromJson(const std::string& path) {
	Json::Value root;
	Json::Reader reader;
	ifstream ifs(path, ifstream::binary);

	if(!ifs.is_open()) {
		cerr << "Can't find file: " << path << endl;
		throw file_not_found(path);
	}
	
	if (!reader.parse(ifs, root, false))
	{
		cerr << "Parsing error: " << reader.getFormattedErrorMessages() << endl;
		return false;
	}

	if(!root.isArray()) {
		cerr << "Expected Array in input.json" << endl;
		return false;
	}
	
	int index = 0;
	for (auto it: root)
	{
		if(!it.isObject()) {
			cerr << "Expected Object in input.json" << endl;
			return false;
		}
		std::string joystick = it["Joystick"].asString();
		if(!joystick.empty()) {
			auto &config = m_storedJoystickConfigs[index];
			config.configured = true;
			config.name = joystick;
			config.bombButton = it["BOMB"].asInt();
			config.skillButton = it["SKILL"].asInt();
			config.xAxis = getAxis(it["X-Axis"].asString().c_str());
			config.yAxis = getAxis(it["Y-Axis"].asString().c_str());
			config.scaleX = it["scaleX"].asFloat() / 100.0f;
			config.scaleY = it["scaleY"].asFloat() / 100.0f;
			config.playerIndex = index;
		} else {
			bindKey(index, PlayerButton::BOMB, getKeyCode(it["BOMB"].asString().c_str()));
			bindKey(index, PlayerButton::SKILL, getKeyCode(it["SKILL"].asString().c_str()));
			bindKey(index, PlayerButton::UP, getKeyCode(it["UP"].asString().c_str()));
			bindKey(index, PlayerButton::DOWN, getKeyCode(it["DOWN"].asString().c_str()));
			bindKey(index, PlayerButton::LEFT, getKeyCode(it["LEFT"].asString().c_str()));
			bindKey(index, PlayerButton::RIGHT, getKeyCode(it["RIGHT"].asString().c_str()));
		}
		
		index++;
		if(index >= MAX_PLAYER_INPUTS)
			break;
	}

	return true;
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

void InputManager::receive(const sf::Event& evt)
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

void InputManager::receive(const MenuShowEvent &evt)
{
	if (evt.visible)
		GameGlobals::events->unsubscribe<sf::Event>(*this);
	else
		GameGlobals::events->subscribe<sf::Event>(*this);
}

void InputManager::update()
{
	for(int i=0; i<sf::Joystick::Count; i++)
	{
		auto *config = m_joystickMap[i];
		if(config)
		{
			auto &player = m_playerInputs[config->playerIndex];
			player.moveX = sf::Joystick::getAxisPosition(i, config->xAxis) * config->scaleX;
			player.moveY = sf::Joystick::getAxisPosition(i, config->yAxis) * config->scaleY;
		}
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
//	cout << "Key pressed: " << getKeyName(code) << endl;
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
//	cout << "Key released: " << getKeyName(code) << endl;
}

void InputManager::updatePlayerMove(PlayerInput &playerInput)
{
	bool up = playerInput.buttonPressed[PlayerButton::UP];
	bool down = playerInput.buttonPressed[PlayerButton::DOWN];
	bool left = playerInput.buttonPressed[PlayerButton::LEFT];
	bool right = playerInput.buttonPressed[PlayerButton::RIGHT];

	if (left != right)
		playerInput.moveX = left ? -1.f : 1.f;
	else
		playerInput.moveX = 0;

	if (up != down)
		playerInput.moveY = up ? -1.f : 1.f;
	else
		playerInput.moveY = 0;

	// Normalize
	if (playerInput.moveX && playerInput.moveY)
	{
		float len = sqrtf(playerInput.moveX * playerInput.moveX + playerInput.moveY * playerInput.moveY);
		playerInput.moveX /= len;
		playerInput.moveY /= len;
	}
//	cout << playerInput.moveX << "/" << playerInput.moveY << endl;
}

void InputManager::onJoystickButtonPressed(int id, int button)
{
	auto *config = m_joystickMap[id];
	if(config) {
		auto &player = m_playerInputs[config->playerIndex];
		if(config->bombButton == button)
			player.buttonPressed[PlayerButton::BOMB] = true;
		else if(config->skillButton == button)
			player.buttonPressed[PlayerButton::SKILL] = true;
	}
}

void InputManager::onJoystickButtonReleased(int id, int button)
{
//	cout << "Joystick button released: " << id << "/" << button << endl;
}

void InputManager::onJoystickConnected(int id)
{
	sf::Joystick::Identification info = sf::Joystick::getIdentification(id);
	auto name = info.name.toAnsiString();
	cout << "Joystick connected: " << name << endl;
	cout << "Buttons: " << sf::Joystick::getButtonCount(id) << endl;
	cout << "Axes: ";
	bool hasAxes = false;
	for(int j = 0; j<sf::Joystick::AxisCount; j++)
	{
		if(sf::Joystick::hasAxis(id, (sf::Joystick::Axis)j))
		{
			if(hasAxes)
				cout << ", ";
			cout << axisNames[j];
			hasAxes = true;
		}
	}
	if(!hasAxes)
		cout << "None";
	cout << endl;
	
	for(int i=0; i<MAX_PLAYER_INPUTS; i++)
	{
		auto &config = m_storedJoystickConfigs[i];
		if(config.configured && name == config.name) {
			m_joystickMap[id] = &config;
			break;
		}
	}
}

void InputManager::onJoystickDisconnected(int id)
{
	sf::Joystick::Identification info = sf::Joystick::getIdentification(id);
	cout << "Joystick disconnected: " << info.name.toAnsiString() << endl;
	m_joystickMap[id] = nullptr;
}

// Keep in sync with sf::Keyboard::Key
static const char *keyNames[] = {
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
	"Left Control",
	"Left Shift",
	"Left Alt",
	"Left Super",
	"Right Control",
	"Right Shift",
	"Right Alt",
	"Right Super",
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
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Insert",
	"Delete",
	"+",
	"-",
	"*",
	"/",
	"Left Arrow",
	"Right Arrow",
	"Up Arrow",
	"Down Arrow",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
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

static const int keyNamesLength = (sizeof (keyNames) / sizeof (*keyNames));
static_assert(keyNamesLength == sf::Keyboard::Key::KeyCount, "sf::Keyboard::Key::KeyCount does not match expectations");

const char *InputManager::getKeyName(int code)
{
	if (code >= 0 && code < sizeof (keyNames))
		return keyNames[code];
	return nullptr;
}

int InputManager::getKeyCode(const char *name)
{
	if (name)
	{
		for (int i = 0; i < keyNamesLength; i++)
		{
			if (strcmp(keyNames[i], name) == 0)
				return i;
		}
	}
	return -1;
}

sf::Joystick::Axis InputManager::getAxis(const char *name)
{
	if (name)
	{
		for (int i = 0; i < sf::Joystick::AxisCount; i++)
		{
			if (strcmp(axisNames[i], name) == 0)
				return (sf::Joystick::Axis)i;
		}
	}
	return (sf::Joystick::Axis)-1;
}
