#include "NetClient.h"
#include "NetConstants.h"
#include "../Events/ChatEvent.h"
#include <iostream>
#include "../GameGlobals.h"
#include "../Events/PlayerJoinEvent.h"
#include "../Game.h"
#include "../Components/InputComponent.h"
#include "../Events/ExitEvent.h"

using namespace std;
using namespace NetCode;

NetClient::NetClient()
	: m_messageWriter(1024)
{
	GameGlobals::events->subscribe<SendChatEvent>(*this);
	m_handler.setCallback(MessageType::HANDSHAKE, &NetClient::onHandshakeMessage, this);
	m_handler.setCallback(MessageType::PLAYER_ID, &NetClient::onPlayerIdMessage, this);
	m_handler.setCallback(MessageType::CHAT, &NetClient::onChatMessage, this);
	m_handler.setCallback(MessageType::PLAYER_JOINED, &NetClient::onPlayerJoinedMessage, this);
	m_handler.setCallback(MessageType::CREATE_SOLID_BLOCK, &NetClient::onCreateSolidBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_BLOCK, &NetClient::onCreateBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_FLOOR, &NetClient::onCreateFloorMessage, this);
	m_handler.setCallback(MessageType::CREATE_PLAYER, &NetClient::onCreatePlayerMessage, this);
	m_handler.setCallback(MessageType::CREATE_BOMB, &NetClient::onCreateBombMessage, this);
	m_handler.setCallback(MessageType::CREATE_EXPLOSION, &NetClient::onCreateExplosionMessage, this);
	m_handler.setCallback(MessageType::DESTROY_ENTITY, &NetClient::onDestroyEntityMessage, this);
	m_handler.setCallback(MessageType::UPDATE_DYNAMIC, &NetClient::onUpdateDynamicMessage, this);
	
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([this](ENetEvent &event)
	{
		cout << "Connected to server!" << endl;

		m_messageWriter.init(MessageType::HANDSHAKE);
		m_messageWriter.write<string>("Santo");
		send(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "Disconnected from server!" << endl;
	});
}

NetClient::~NetClient()
{
	GameGlobals::events->unsubscribe<SendChatEvent>(*this);
	m_connection.disconnectNow();
	cout << "Connection closed" << endl;
}

void NetClient::update()
{
	if (m_playerEntity.valid())
	{
		auto input = m_playerEntity.component<InputComponent>();
		if (input->bombButtonPressed)
		{
			sendInputEventMessage(MessageType::INPUT_BOMB_ACTIVATED);
			input->bombButtonPressed = false;
		}
		if (input->skillButtonPressed)
		{
			sendInputEventMessage(MessageType::INPUT_SKILL_ACTIVATED);
			input->skillButtonPressed = false;
		}

		m_messageWriter.init(MessageType::INPUT_DIRECTION);
		m_messageWriter.write<float>(input->moveX);
		m_messageWriter.write<float>(input->moveY);
		send(NetChannel::INPUT_UNRELIABLE, m_messageWriter.createPacket(0));
	}

	if (!m_connection.update())
		cout << "Error during host service" << endl; //fixme: count errors, if too many disconnect
}

void NetClient::sendInputEventMessage(MessageType type)
{
	m_messageWriter.init(type);
	send(NetChannel::INPUT_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

bool NetClient::connect(const std::string &hostname, int port)
{
	return m_connection.connect(hostname, port, (int)NetChannel::COUNT);
}

void NetClient::disconnect()
{
	m_connection.disconnect();
}

void NetClient::receive(const SendChatEvent& evt)
{
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(evt.message);
	send(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetClient::onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t width = reader.read<uint8_t>();
	uint8_t height = reader.read<uint8_t>();
	GameGlobals::game->init(width, height);
}

void NetClient::onPlayerIdMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	m_playerEntity = getEntity(id);
	m_playerEntity.component<InputComponent>()->playerIndex = 0;
}

void NetClient::onChatMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	string msg = reader.read<string>();
	string name = reader.read<string>();
	GameGlobals::events->emit<ChatEvent>(msg, name);
}

void NetClient::onPlayerJoinedMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	string name = reader.read<string>();
	GameGlobals::events->emit<PlayerJoinEvent>(name);
}

void NetClient::onCreateSolidBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	mapEntity(id, GameGlobals::entityFactory->createSolidBlock(x, y));
}

void NetClient::onCreateBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	mapEntity(id, GameGlobals::entityFactory->createBlock(x, y));
}

void NetClient::onCreateFloorMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	mapEntity(id, GameGlobals::entityFactory->createFloor(x, y));
}

void NetClient::onCreatePlayerMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	float x = reader.read<float>();
	float y = reader.read<float>();
	mapEntity(id, GameGlobals::entityFactory->createPlayer(x, y));
}

void NetClient::onCreateBombMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	uint64_t ownerId = reader.read<uint64_t>();
	Entity owner = getEntity(ownerId);
	if (owner.valid())
		mapEntity(id, GameGlobals::entityFactory->createBomb(x, y, owner));
}

void NetClient::onCreateExplosionMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	Direction direction = reader.read<Direction>();
	uint8_t range = reader.read<uint8_t>();
	float spreadTime = reader.read<float>();
	mapEntity(id, GameGlobals::entityFactory->createExplosion(x, y, direction, range, spreadTime));
}

void NetClient::onDestroyEntityMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	Entity entity = getEntity(id);
	if (entity.valid()) {
		entity.destroy();
		entityMap.erase(id);
	}
}

void NetClient::onUpdateDynamicMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	float x = reader.read<float>();
	float y = reader.read<float>();
	Entity entity = getEntity(id);
	if (entity.valid())
	{
		auto transform = entity.component<TransformComponent>();
		if(transform.valid())
		{
			transform->x = x;
			transform->y = y;
		}
		if(entity != m_playerEntity) {
			auto input = entity.component<InputComponent>();
			if(input.valid())
			{
				input->moveX = reader.read<float>();
				input->moveY = reader.read<float>();
			}
		}
	}
}

Entity NetClient::getEntity(uint64_t id)
{
	auto it = entityMap.find(id);
	if (it == entityMap.end())
	{
		// Fixme: Id does not exist, show error, disconnect
		cerr << "Error: Id does not exist" << endl;
//		exit(EXIT_FAILURE);
		return Entity();
	}
	return it->second;
}

void NetClient::send(NetChannel channel, ENetPacket* packet)
{
	enet_peer_send(m_connection.getPeer(), (enet_uint8)channel, packet);
}

void NetClient::mapEntity(uint64_t id, Entity entity)
{
	if (entityMap.count(id) != 0)
	{
		// Fixme: Id already exists, show error, disconnect
		cerr << "Error: Id already exists" << endl;
		GameGlobals::events->emit<ExitEvent>();
		return;
	}
	entityMap[id] = entity;
}