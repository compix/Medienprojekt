#include "NetClient.h"
#include "NetConstants.h"
#include "../Events/ChatEvent.h"
#include <iostream>
#include "../GameGlobals.h"
#include "../Events/PlayerJoinEvent.h"
#include "../Game.h"
#include "../Components/InputComponent.h"
#include "../Events/ExitEvent.h"
#include "../Events/DeathEvent.h"
#include "../Components/LocalInputComponent.h"
#include "../Events/ClientStateEvent.h"
#include "../Events/ForceDisconnectEvent.h"
#include "../Events/LobbyEvent.h"
#include "../Events/CountdownEvent.h"
#include "../Events/ReadyEvent.h"
#include "../Events/SendChatEvent.h"
#include "../Events/SetReadyEvent.h"
#include "../Events/StartGameEvent.h"
#include "../Components/DynamicComponent.h"
#include "../Events/GameOverEvent.h"
#include "../Events/ResetGameEvent.h"

using namespace std;
using namespace NetCode;

NetClient::NetClient()
	: m_messageWriter(1024)
{
	GameGlobals::events->subscribe<SendChatEvent>(*this);
	GameGlobals::events->subscribe<SetReadyEvent>(*this);
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
	m_handler.setCallback(MessageType::GAME_OVER, &NetClient::onGameOverMessage, this);
	m_handler.setCallback(MessageType::RESET_GAME, &NetClient::onResetGameMessage, this);
	
	
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([this](ENetEvent &event)
	{
		GameGlobals::events->emit<ClientStateEvent>("Connection established", ClientState::CONNECTED);
		cout << "Connected to server!" << endl;

		m_messageWriter.init(MessageType::HANDSHAKE);
		m_messageWriter.write<string>("Santo");
		send(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "Disconnected from server!" << endl;
		if (!event.peer)
			GameGlobals::events->emit<ClientStateEvent>("Connection failed", ClientState::DISCONNECTED);
		GameGlobals::events->emit<ForceDisconnectEvent>(event.peer ? "Server disconnected" : "Connection failed");
	});
}

NetClient::~NetClient()
{
	GameGlobals::events->unsubscribe<SendChatEvent>(*this);
	m_connection.disconnectNow();
	cout << "Connection closed" << endl;
}

void NetClient::update(float deltaTime)
{
	m_nextSend -= deltaTime;
	if (m_nextSend <= 0 && m_playerEntity.valid())
	{
		m_nextSend = 0.016f;
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

void NetClient::receive(const SendChatEvent& evt)
{
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(evt.message);
	send(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetClient::receive(const SetReadyEvent& evt)
{
	if (evt.playerIndex == m_playerIndex)
	{
		m_messageWriter.init(MessageType::PLAYER_READY);
		m_messageWriter.write<bool>(evt.ready);
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
		GameGlobals::events->emit<ClientStateEvent>("Received handshake", ClientState::PREGAME);
	else if (status == ServerStatus::LOBBY || status == ServerStatus::LOBBY_COUNTDOWN)
	{
		GameGlobals::events->emit<ClientStateEvent>("Received handshake", ClientState::LOBBY);
		GameGlobals::events->emit(lobbyEvt);
	}
	// fixme: else
}

void NetClient::onPlayerReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t playerIndex = reader.read<uint8_t>();
	bool ready = reader.read<bool>();
	GameGlobals::events->emit<ReadyEvent>(playerIndex, ready);
}

void NetClient::onStartGameMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	m_playerEntity = getEntity(id);
	if (m_playerEntity.valid())
		m_playerEntity.assign<LocalInputComponent>(0);

	GameGlobals::events->emit<StartGameEvent>();
}

void NetClient::onChatMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	string msg = reader.read<string>();
	string name = reader.read<string>();
	GameGlobals::events->emit<ChatEvent>(msg, name);
}

void NetClient::onPlayerJoinedMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t playerIndex = reader.read<uint8_t>();
	string name = reader.read<string>();
	GameGlobals::events->emit<PlayerJoinEvent>(playerIndex, name);
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
	bool ghost = reader.read<bool>();
	bool lightning = reader.read<bool>();
	Entity owner = getEntity(ownerId);
	if (owner.valid())
		mapEntity(id, GameGlobals::entityFactory->createBomb(x, y, owner, ghost, lightning));
}

void NetClient::onCreateExplosionMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	Direction direction = reader.read<Direction>();
	uint8_t range = reader.read<uint8_t>();
	float spreadTime = reader.read<float>();
	bool ghost = reader.read<bool>();
	bool lightning = reader.read<bool>();
	mapEntity(id, GameGlobals::entityFactory->createExplosion(x, y, direction, range, spreadTime, ghost, lightning));
}

void NetClient::onCreatePortalMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	uint64_t ownerId = reader.read<uint64_t>();
	Entity owner = getEntity(ownerId);
	if (owner.valid())
		mapEntity(id, GameGlobals::entityFactory->createPortal(x, y, owner, false));
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
	Entity target = getEntity(targetId);
	if (target.valid())
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
	Entity entity = getEntity(id);
	if (entity.valid())
		GameGlobals::events->emit<DeathEvent>(entity);
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
	uint64_t packetNumber = reader.read<uint64_t>();
	float x = reader.read<float>();
	float y = reader.read<float>();
	Entity entity = getEntity(id);
	if (entity.valid())
	{
		auto dynamic = entity.component<DynamicComponent>();
		if (dynamic->packetNumber >= packetNumber)
			return;
		dynamic->packetNumber = packetNumber;
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

void NetClient::onCountdownMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	GameGlobals::events->emit<CountdownEvent>(reader.read<string>());
}

void NetClient::onAllReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	GameGlobals::events->emit<LobbyEventDisable>();
}

void NetClient::onGameOverMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint64_t id = reader.read<uint64_t>();
	GameGlobals::events->emit<GameOverEvent>(getEntity(id));
}

void NetClient::onResetGameMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	GameGlobals::events->emit<ResetGameEvent>();
	entityMap.clear();
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