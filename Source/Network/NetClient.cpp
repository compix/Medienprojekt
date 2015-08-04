#include "NetClient.h"
#include "NetConstants.h"

#include <iostream>
#include "../GameGlobals.h"
#include "../Game.h"
#include "../Components/InputComponent.h"
#include "../Components/LocalInputComponent.h"
#include "../Components/DynamicComponent.h"
#include <ecstasy/core/Engine.h>

using namespace std;
using namespace NetCode;

NetClient::NetClient()
	: m_messageWriter(1024)
{
	m_connections += GameGlobals::events->sendChat.connect(this, NetClient::onSendChat);
	m_connections += GameGlobals::events->setReady.connect(this, NetClient::onSetReady);
	m_handler.setCallback(MessageType::HANDSHAKE, &NetClient::onHandshakeMessage, this);
	m_handler.setCallback(MessageType::START_GAME, &NetClient::onStartGameMessage, this);
	m_handler.setCallback(MessageType::PLAYER_READY, &NetClient::onPlayerReadyMessage, this);
	m_handler.setCallback(MessageType::CHAT, &NetClient::onChatMessage, this);
	m_handler.setCallback(MessageType::PLAYER_JOINED, &NetClient::onPlayerJoinedMessage, this);
	m_handler.setCallback(MessageType::CREATE_SOLID_BLOCK, &NetClient::onCreateSolidBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_BLOCK, &NetClient::onCreateBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_FLOOR, &NetClient::onCreateFloorMessage, this);
	m_handler.setCallback(MessageType::CREATE_PLAYER, &NetClient::onCreatePlayerMessage, this);
	m_handler.setCallback(MessageType::CREATE_BOMB, &NetClient::onCreateBombMessage, this);
	m_handler.setCallback(MessageType::CREATE_EXPLOSION, &NetClient::onCreateExplosionMessage, this);
	m_handler.setCallback(MessageType::CREATE_PORTAL, &NetClient::onCreatePortalMessage, this);
	m_handler.setCallback(MessageType::CREATE_ITEM, &NetClient::onCreateItemMessage, this);
	m_handler.setCallback(MessageType::CREATE_BOOST_EFFECT, &NetClient::onCreateBoostEffectMessage, this);
	m_handler.setCallback(MessageType::CREATE_SMOKE, &NetClient::onCreateSmokeMessage, this);
	m_handler.setCallback(MessageType::DEATH, &NetClient::onDeathMessage, this);
	m_handler.setCallback(MessageType::DESTROY_ENTITY, &NetClient::onDestroyEntityMessage, this);
	m_handler.setCallback(MessageType::UPDATE_DYNAMIC, &NetClient::onUpdateDynamicMessage, this);
	m_handler.setCallback(MessageType::COUNTDOWN, &NetClient::onCountdownMessage, this);
	m_handler.setCallback(MessageType::ALL_READY, &NetClient::onAllReadyMessage, this);
	
	
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([this](ENetEvent &event)
	{
		GameGlobals::events->clientState.emit("Connection established", ClientState::CONNECTED);
		cout << "Connected to server!" << endl;

		m_messageWriter.init(MessageType::HANDSHAKE);
		m_messageWriter.write<string>("Santo");
		send(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "Disconnected from server!" << endl;
		if (!event.peer)
			GameGlobals::events->clientState.emit("Connection failed", ClientState::DISCONNECTED);
		GameGlobals::events->forceDisconnect.emit(event.peer ? "Server disconnected" : "Connection failed");
	});
}

NetClient::~NetClient()
{
	m_connections.removeAll();
	m_connection.disconnectNow();
	cout << "Connection closed" << endl;
}

void NetClient::update(float deltaTime)
{
	m_nextSend -= deltaTime;
	Entity *playerEntity = GameGlobals::engine->getEntity(m_playerEntityId);
	if (m_nextSend <= 0 && playerEntity)
	{
		m_nextSend = 0.016f;
		auto input = playerEntity->get<InputComponent>();
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
		m_messageWriter.write<uint64_t>(input->packetNumber++);
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

void NetClient::onSendChat(const string &message)
{
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(message);
	send(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetClient::onSetReady(int playerIndex, bool ready)
{
	if (playerIndex == m_playerIndex)
	{
		m_messageWriter.init(MessageType::PLAYER_READY);
		m_messageWriter.write<bool>(ready);
		send(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	}
}

void NetClient::onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	ServerStatus status = reader.read<ServerStatus>();
	m_playerIndex = reader.read<uint8_t>();
	uint8_t width = reader.read<uint8_t>();
	uint8_t height = reader.read<uint8_t>();
	uint8_t numPlayers = reader.read<uint8_t>();

	LobbyEvent lobbyEvt(numPlayers);
	for (int i = 0; i < numPlayers; i++)
	{
		lobbyEvt.name[i] = reader.read<string>();
		lobbyEvt.ready[i] = reader.read<bool>();
		lobbyEvt.enabled[i] = i == m_playerIndex;
	}

	GameGlobals::game->init(width, height);

	if (status == ServerStatus::INGAME)
		GameGlobals::events->clientState.emit("Received handshake", ClientState::PREGAME);
	else if (status == ServerStatus::LOBBY || status == ServerStatus::LOBBY_COUNTDOWN)
	{
		GameGlobals::events->clientState.emit("Received handshake", ClientState::LOBBY);
		GameGlobals::events->lobby.emit(lobbyEvt);
	}
	// fixme: else
}

void NetClient::onPlayerReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t playerIndex = reader.read<uint8_t>();
	bool ready = reader.read<bool>();
	GameGlobals::events->ready.emit(playerIndex, ready);
}

void NetClient::onStartGameMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	Entity *playerEntity = getEntity(id);
	if (playerEntity && playerEntity->isValid()) {
		playerEntity->add(GameGlobals::engine->createComponent<LocalInputComponent>());
		m_playerEntityId = playerEntity->getId();
	}

	GameGlobals::events->startGame.emit();
}

void NetClient::onChatMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	string msg = reader.read<string>();
	string name = reader.read<string>();
	GameGlobals::events->chat.emit(msg, name);
}

void NetClient::onPlayerJoinedMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t playerIndex = reader.read<uint8_t>();
	string name = reader.read<string>();
	GameGlobals::events->playerJoin.emit(playerIndex, name);
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
	int playerIndex = reader.read<uint8_t>();
	float x = reader.read<float>();
	float y = reader.read<float>();
	mapEntity(id, GameGlobals::entityFactory->createPlayer(x, y, playerIndex));
}

void NetClient::onCreateBombMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	uint64_t ownerId = reader.read<uint64_t>();
	Entity *owner = getEntity(ownerId);
	if (owner && owner->isValid())
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

void NetClient::onCreatePortalMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	uint64_t ownerId = reader.read<uint64_t>();
	Entity *owner = getEntity(ownerId);
	if (owner && owner->isValid())
		mapEntity(id, GameGlobals::entityFactory->createPortal(x, y, owner));
}

void NetClient::onCreateItemMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	ItemType type = reader.read<ItemType>();
	mapEntity(id, GameGlobals::entityFactory->createItem(x, y, type));
}

void NetClient::onCreateBoostEffectMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	uint64_t targetId = reader.read<uint64_t>();
	Entity *target = getEntity(targetId);
	if (target && target->isValid())
		mapEntity(id, GameGlobals::entityFactory->createBoostEffect(x, y, target));
}

void NetClient::onCreateSmokeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	mapEntity(id, GameGlobals::entityFactory->createSmoke(x, y));
}

void NetClient::onDeathMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	Entity *entity = getEntity(id);
	if (entity && entity->isValid())
		GameGlobals::events->death.emit(entity);
}

void NetClient::onDestroyEntityMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	Entity *entity = getEntity(id);
	if (entity && entity->isValid()) {
		GameGlobals::engine->removeEntity(entity);
		entityMap.erase(id);
	}
}

void NetClient::onUpdateDynamicMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint64_t packetNumber = reader.read<uint64_t>();
	float x = reader.read<float>();
	float y = reader.read<float>();
	Entity *entity = getEntity(id);
	if (entity && entity->isValid())
	{
		auto dynamic = entity->get<DynamicComponent>();
		if (dynamic->packetNumber >= packetNumber)
			return;
		dynamic->packetNumber = packetNumber;
		auto transform = entity->get<TransformComponent>();
		if(transform)
		{
			transform->x = x;
			transform->y = y;
		}
		if(entity->getId() != m_playerEntityId) {
			auto input = entity->get<InputComponent>();
			if(input)
			{
				input->moveX = reader.read<float>();
				input->moveY = reader.read<float>();
			}
		}
	}
}

void NetClient::onCountdownMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	GameGlobals::events->countdown.emit(reader.read<string>());
}

void NetClient::onAllReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	GameGlobals::events->lobbyDisable.emit();
}

Entity *NetClient::getEntity(uint64_t id)
{
	auto it = entityMap.find(id);
	if (it == entityMap.end())
	{
		// Fixme: Id does not exist, show error, disconnect
		cerr << "Error: Id does not exist" << endl;
//		exit(EXIT_FAILURE);
		return nullptr;
	}
	return it->second;
}

void NetClient::send(NetChannel channel, ENetPacket* packet)
{
	enet_peer_send(m_connection.getPeer(), (enet_uint8)channel, packet);
}

void NetClient::mapEntity(uint64_t id, Entity *entity)
{
	if (entityMap.count(id) != 0)
	{
		// Fixme: Id already exists, show error, disconnect
		cerr << "Error: Id already exists" << endl;
		GameGlobals::events->exit.emit();
		return;
	}
	entityMap[id] = entity;
}