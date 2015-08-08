#include "NetServer.h"
#include <iostream>
#include "NetConstants.h"
#include "NetPlayerInfo.h"
#include "../Events/ChatEvent.h"
#include "../Events/DisconnectEvent.h"
#include "../GameGlobals.h"
#include "../Game.h"
#include "../Events/ExitEvent.h"
#include "../Events/PlayerJoinEvent.h"
#include "../Events/SendChatEvent.h"
#include "../Events/BombCreatedEvent.h"
#include "../Components/SolidBlockComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/FloorComponent.h"
#include "../Components/BlockComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/OwnerComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/DynamicComponent.h"
#include "../Components/PortalComponent.h"
#include "../Components/ItemComponent.h"
#include "../Events/ExplosionCreatedEvent.h"
#include "../Events/PortalCreatedEvent.h"
#include "../Events/ItemCreatedEvent.h"
#include "../Events/BoostEffectCreatedEvent.h"
#include "../Events/SmokeCreatedEvent.h"
#include "../Events/DeathEvent.h"
#include "../Events/SetReadyEvent.h"
#include "../Events/LobbyEvent.h"
#include "../Events/CountdownEvent.h"
#include "../Components/FreeSlotComponent.h"
#include "../Events/StartGameEvent.h"
#include "../Components/PlayerComponent.h"
#include "../Events/ReadyEvent.h"
#include <format.h>
#include "../Events/GameOverEvent.h"
#include "../Events/ResetGameEvent.h"

using namespace std;
using namespace NetCode;

NetServer::NetServer()
	: m_messageWriter(1024)
{
	GameGlobals::events->subscribe<SendChatEvent>(*this);
	GameGlobals::events->subscribe<BombCreatedEvent>(*this);
	GameGlobals::events->subscribe<ExplosionCreatedEvent>(*this);
	GameGlobals::events->subscribe<EntityDestroyedEvent>(*this);
	GameGlobals::events->subscribe<PortalCreatedEvent>(*this);
	GameGlobals::events->subscribe<ItemCreatedEvent>(*this);
	GameGlobals::events->subscribe<BoostEffectCreatedEvent>(*this);
	GameGlobals::events->subscribe<SmokeCreatedEvent>(*this);
	GameGlobals::events->subscribe<DeathEvent>(*this);
	GameGlobals::events->subscribe<SetReadyEvent>(*this);
	GameGlobals::events->subscribe<GameOverEvent>(*this);
	GameGlobals::events->subscribe<ResetGameEvent>(*this);
	GameGlobals::events->subscribe<StartGameEvent>(*this);

	m_handler.setCallback(MessageType::HANDSHAKE, &NetServer::onHandshakeMessage, this);
	m_handler.setCallback(MessageType::INPUT_DIRECTION, &NetServer::onInputDirectionMessage, this);
	m_handler.setCallback(MessageType::INPUT_BOMB_ACTIVATED, &NetServer::onInputBombActivatedMessage, this);
	m_handler.setCallback(MessageType::INPUT_SKILL_ACTIVATED, &NetServer::onInputSkillActivatedMessage, this);
	m_handler.setCallback(MessageType::CHAT, &NetServer::onChatMessage, this);
	m_handler.setCallback(MessageType::PLAYER_READY, &NetServer::onPlayerReadyMessage, this);

	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([&](ENetEvent &evt)
	{
		cout << "A client connected!" << endl;
		if (m_status != ServerStatus::SHUTDOWN)
		{
			for (int i = 0; i < m_numPlayers; i++)
			{
				if (m_playerInfos[i].status == NetPlayerStatus::DISCONNECTED)
				{
					evt.peer->data = &m_playerInfos[i];
					m_playerInfos[i].init(evt.peer);
					return;
				}
			}
		}
		enet_peer_disconnect(evt.peer, 0);
	});

	m_connection.setDisconnectCallback([](ENetEvent &evt)
	{
		if (evt.peer->data)
		{
			NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
			if (info->status == NetPlayerStatus::CONNECTING)
				GameGlobals::events->emit<DisconnectEvent>("The client was unable to connect", info);
			else
			{
				GameGlobals::events->emit<DisconnectEvent>("The client disconnected", info);
				if (info->entity.valid())
					info->entity.assign<FreeSlotComponent>();
			}
			cout << "A client disconnected!" << endl;
			evt.peer->data = nullptr;
			info->invalidate();
		}
	});
}

NetServer::~NetServer()
{
	GameGlobals::events->unsubscribe<SendChatEvent>(*this);
	GameGlobals::events->unsubscribe<BombCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<ExplosionCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<EntityDestroyedEvent>(*this);
	GameGlobals::events->unsubscribe<PortalCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<ItemCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<BoostEffectCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<SmokeCreatedEvent>(*this);
	GameGlobals::events->unsubscribe<DeathEvent>(*this);
	GameGlobals::events->unsubscribe<SetReadyEvent>(*this);

	// Delete all playerinfos
	auto host = m_connection.getHost();
	for (size_t i = 0; i < host->peerCount; i++)
	{
		auto &peer = host->peers[i];
		if (peer.data)
		{
			NetPlayerInfo *info = static_cast<NetPlayerInfo *>(peer.data);
			peer.data = nullptr;
		}
	}
	m_connection.disconnectNow();
	cout << "Connection closed" << endl;
}

void NetServer::update(float deltaTime)
{
	m_nextBroadcast -= deltaTime;
	if (m_nextBroadcast <= 0)
	{
		m_nextBroadcast = 0.016f;
		broadcastDynamicUpdates();
	}

	if (!m_connection.update())
		cout << "Error during host service" << endl; //fixme: count errors, if too many disconnect

	// Do countdowns
	if ((m_status == ServerStatus::LOBBY || m_status == ServerStatus::LOBBY_COUNTDOWN) && m_countdown > 0)
	{
		auto lower = floor(m_countdown);
		m_countdown -= deltaTime;
		if (m_countdown <= 0)
		{
			if (m_status == ServerStatus::LOBBY)
				forceReady();
			else
				startGame();
		}
		else if (m_countdown <= lower)
		{
			std::string message;
			if (m_status == ServerStatus::LOBBY)
				message = fmt::format("Get Ready: {}", (int)lower);
			else
				message = fmt::format("Starting: {}", (int)lower);

			m_messageWriter.init(MessageType::COUNTDOWN);
			m_messageWriter.write<string>(message);
			broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));

			GameGlobals::events->emit<CountdownEvent>(message);
		}
	}
}

bool NetServer::connect(const CreateGameEvent& evt)
{
	if (!m_connection.connect("", evt.port, NetConstants::MAX_CLIENTS, (enet_uint8)NetChannel::COUNT))
		return false;

	m_countdown = GameConstants::LOBBY_COUNTDOWN;
	m_width = evt.width;
	m_height = evt.height;

	m_numPlayers = evt.players.size();
	for (int i = 0; i < m_numPlayers; i++)
	{
		auto &player = evt.players[i];

		if (player.type == CreateGamePlayerType::CLIENT)
			m_playerInfos[i].invalidate();
		else
		{
			if (player.type == CreateGamePlayerType::COMPUTER)
				m_playerInfos[i].status = NetPlayerStatus::READY;
			else
				m_playerInfos[i].status = NetPlayerStatus::CONNECTED;
			m_playerInfos[i].name = player.name;
			makeUniqueName(&m_playerInfos[i]);
			m_playerInfos[i].entity.invalidate();
		}
		m_playerInfos[i].type = player.type;
		m_playerInfos[i].playerIndex = i;
	}

	m_status = ServerStatus::LOBBY;

	return true;
}

void NetServer::disconnect()
{
	m_connection.disconnect();
}

void NetServer::receive(const SendChatEvent& evt)
{
	string serverName = m_playerInfos[0].name;
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(evt.message);
	m_messageWriter.write<string>(serverName);
	broadcast(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	GameGlobals::events->emit<ChatEvent>(evt.message, serverName);
}

void NetServer::receive(const BombCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createBombPacket(evt.entity, evt.x, evt.y, evt.owner));
}

void NetServer::receive(const ExplosionCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createExplosionPacket(evt.entity, evt.x, evt.y, evt.direction, evt.range, evt.spreadTime));
}

void NetServer::receive(const EntityDestroyedEvent& evt)
{
	m_messageWriter.init(MessageType::DESTROY_ENTITY);
	m_messageWriter.write<uint64_t>(evt.entity.id().id());
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::receive(const PortalCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createPortalPacket(evt.entity, evt.x, evt.y, evt.owner));
}

void NetServer::receive(const ItemCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createItemPacket(evt.entity, evt.x, evt.y, evt.type));
}

void NetServer::receive(const BoostEffectCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createBoostEffectPacket(evt.entity, evt.x, evt.y, evt.target));
}

void NetServer::receive(const SmokeCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD_RELIABLE, createSmokePacket(evt.entity, evt.x, evt.y));
}

void NetServer::receive(const DeathEvent& evt)
{
	m_messageWriter.init(MessageType::DEATH);
	m_messageWriter.write<uint64_t>(evt.dyingEntity.id().id());
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::receive(const SetReadyEvent& evt)
{
	if (m_status != ServerStatus::LOBBY)
		return;

	m_playerInfos[evt.playerIndex].status = evt.ready ? NetPlayerStatus::READY : NetPlayerStatus::CONNECTED;
	broadcastPlayerReady(evt.playerIndex, evt.ready);
	bool allReady = allPlayersReady();
	emitLobbyEvent(allReady);
	if (allReady)
		startCountdown();
}

void NetServer::receive(const GameOverEvent& evt)
{
	m_messageWriter.init(MessageType::GAME_OVER);
	m_messageWriter.write<uint64_t>(evt.lastPlayer.id().id());
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::receive(const ResetGameEvent& evt)
{
	m_messageWriter.init(MessageType::RESET_GAME);
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::receive(const StartGameEvent& evt)
{
	// Assign entities and send playerids
	for (int i = 0; i < m_numPlayers; i++)
	{
		if (m_playerInfos[i].peer && m_playerInfos[i].status >= NetPlayerStatus::CONNECTED)
		{
			m_playerInfos[i].entity = getFreeSlotEntity();

			// Send all blocks:
			sendBlockEntities<SolidBlockComponent>(m_playerInfos[i].peer, MessageType::CREATE_SOLID_BLOCK);
			sendBlockEntities<FloorComponent>(m_playerInfos[i].peer, MessageType::CREATE_FLOOR);
			sendBlockEntities<BlockComponent>(m_playerInfos[i].peer, MessageType::CREATE_BLOCK);

			// Send players
			sendPlayerEntities(m_playerInfos[i].peer);
			sendStartGame(&m_playerInfos[i]);
		}
	}
}

void NetServer::startCountdown()
{
	if (m_status != ServerStatus::LOBBY)
		return;

	m_status = ServerStatus::LOBBY_COUNTDOWN;
	m_countdown = GameConstants::LOBBY_READY_COUNTDOWN;

	m_messageWriter.init(MessageType::ALL_READY);
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::startGame()
{
	if (m_status != ServerStatus::LOBBY_COUNTDOWN)
		return;

	auto *game = (LocalGame *)GameGlobals::game.get();
	game->resetEntities();
	m_status = ServerStatus::INGAME;
}

void NetServer::broadcast(NetChannel channel, ENetPacket *packet)
{
	enet_host_broadcast(m_connection.getHost(), (enet_uint8)channel, packet);
}

void NetServer::send(ENetPeer* peer, NetChannel channel, ENetPacket *packet)
{
	enet_peer_send(peer, (enet_uint8)channel, packet);
}

void NetServer::sendStartGame(NetPlayerInfo* info)
{
	m_messageWriter.init(MessageType::START_GAME);
	m_messageWriter.write<uint64_t>(info->entity.valid() ? info->entity.id().id() : Entity::INVALID.id());
	send(info->peer, NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

Entity NetServer::getFreeSlotEntity()
{
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components<FreeSlotComponent>())
	{
		entity.remove<FreeSlotComponent>();
		return entity;
	}
	return Entity();
}

void NetServer::emitLobbyEvent(bool disable)
{
	LobbyEvent lobbyEvt(m_numPlayers);
	for (int i = 0; i < m_numPlayers; i++)
	{
		lobbyEvt.enabled[i] = !disable && m_playerInfos[i].type == CreateGamePlayerType::LOCAL;
		lobbyEvt.ready[i] = m_playerInfos[i].status == NetPlayerStatus::READY;
		lobbyEvt.name[i] = m_playerInfos[i].name;
	}
	GameGlobals::events->emit(lobbyEvt);
}

bool NetServer::allPlayersReady()
{
	int ready = 0;
	for (int i = 0; i < m_numPlayers; i++)
	{
		if (m_playerInfos[i].status == NetPlayerStatus::READY || m_playerInfos[i].status == NetPlayerStatus::DISCONNECTED)
			ready++;
	}
	return ready == m_numPlayers;
}

void NetServer::forceReady()
{
	LobbyEvent lobbyEvt(m_numPlayers);
	for (int i = 0; i < m_numPlayers; i++)
	{
		lobbyEvt.enabled[i] = false;
		lobbyEvt.ready[i] = true;
		lobbyEvt.name[i] = m_playerInfos[i].name;
	}
	GameGlobals::events->emit(lobbyEvt);
	startCountdown();
}

void NetServer::makeUniqueName(NetPlayerInfo *info)
{
	bool changed;
	do {
		changed = false;
		for (int i = 0; i < m_numPlayers; i++)
		{
			if (info != &m_playerInfos[i] && info->name == m_playerInfos[i].name)
			{
				info->name += '#';
				changed = true;
			}
		}
	} while (changed);
}

void NetServer::onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	info->status = NetPlayerStatus::CONNECTED;
	info->name = reader.read<string>();
	makeUniqueName(info);
	GameGlobals::events->emit<PlayerJoinEvent>(info->playerIndex, info->name);

	// Send greeting back
	m_messageWriter.init(MessageType::HANDSHAKE);
	m_messageWriter.write<ServerStatus>(m_status);
	m_messageWriter.write<uint8_t>(info->playerIndex);
	m_messageWriter.write<uint8_t>(m_width);
	m_messageWriter.write<uint8_t>(m_height);
	m_messageWriter.write<uint8_t>(m_numPlayers);
	// Assign entities and send playerids
	for (int i = 0; i < m_numPlayers; i++)
	{
		m_messageWriter.write<string>(m_playerInfos[i].name);
		m_messageWriter.write<bool>(m_playerInfos[i].status == NetPlayerStatus::READY);
	}
	send(evt.peer, NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));

	// Notify all playes about the join
	m_messageWriter.init(MessageType::PLAYER_JOINED);
	m_messageWriter.write<uint8_t>(info->playerIndex);
	m_messageWriter.write<string>(info->name);
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));

	// If the client connected after the game started, send all entities
	if (m_status == ServerStatus::INGAME)
	{
		// Send all blocks:
		sendBlockEntities<SolidBlockComponent>(evt.peer, MessageType::CREATE_SOLID_BLOCK);
		sendBlockEntities<FloorComponent>(evt.peer, MessageType::CREATE_FLOOR);
		sendBlockEntities<BlockComponent>(evt.peer, MessageType::CREATE_BLOCK);

		// Send entities to new player
		sendPlayerEntities(evt.peer);
		sendBombEntities(evt.peer);
		sendPortalEntities(evt.peer);
		sendItemEntities(evt.peer);
		//Fixme: boost effects
		sendExplosionEntities(evt.peer);
		sendSmokeEntities(evt.peer);

		Entity playerEntity = getFreeSlotEntity();
		if (playerEntity.valid())
		{
			info->entity = playerEntity;
			playerEntity.component<InputComponent>()->packetNumber = 0;
		}

		sendStartGame(info);
	}

}

void NetServer::onPlayerReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	if (m_status != ServerStatus::LOBBY)
		return;

	bool ready = reader.read<bool>();
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	info->status = ready ? NetPlayerStatus::READY : NetPlayerStatus::CONNECTED;
	GameGlobals::events->emit<ReadyEvent>(info->playerIndex, ready);

	// Let all clients know
	broadcastPlayerReady(info->playerIndex, ready);

	if (allPlayersReady())
		startCountdown();
}

void NetServer::broadcastPlayerReady(uint8_t playerIndex, bool ready)
{
	m_messageWriter.init(MessageType::PLAYER_READY);
	m_messageWriter.write<uint8_t>(playerIndex);
	m_messageWriter.write<bool>(ready);
	broadcast(NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::onInputDirectionMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	if (info->entity.valid()) {
		auto input = info->entity.component<InputComponent>();
		auto packetNumber = reader.read<uint64_t>();
		if (input->packetNumber >= packetNumber)
			return;
		input->packetNumber = packetNumber;
		input->moveX = reader.read<float>();
		input->moveY = reader.read<float>();
	}
}

void NetServer::onInputBombActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	if (info->entity.valid()) {
		auto input = info->entity.component<InputComponent>();
		input->bombButtonPressed = true;
	}
}

void NetServer::onInputSkillActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	if (info->entity.valid()) {
		auto input = info->entity.component<InputComponent>();
		input->skillButtonPressed = true;
	}
}


template<typename T>
void NetServer::sendBlockEntities(ENetPeer *peer, MessageType type)
{
	ComponentHandle<T> block;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(block, cell))
	{
		m_messageWriter.init(type);
		m_messageWriter.write<uint64_t>(entity.id().id());
		m_messageWriter.write<uint8_t>(cell->x);
		m_messageWriter.write<uint8_t>(cell->y);
		send(peer, NetChannel::WORLD_RELIABLE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	}
}

void NetServer::sendPlayerEntities(ENetPeer *peer)
{
	ComponentHandle<InputComponent> input;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<PlayerComponent> player;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(input, transform, player))
		send(peer, NetChannel::WORLD_RELIABLE, createPlayerPacket(entity, transform->x, transform->y, player->index));
}

ENetPacket *NetServer::createPlayerPacket(Entity entity, float x, float y, uint8_t playerIndex)
{
	m_messageWriter.init(MessageType::CREATE_PLAYER);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(playerIndex);
	m_messageWriter.write<float>(x);
	m_messageWriter.write<float>(y);
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::broadcastDynamicUpdates()
{
	ComponentHandle<DynamicComponent> dynamic;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(dynamic, transform, cell))
		broadcast(NetChannel::WORLD_UNRELIABLE, createUpdateDynamicPacket(entity, transform->x, transform->y, dynamic->packetNumber++));
}

ENetPacket *NetServer::createUpdateDynamicPacket(Entity entity, float x, float y, uint64_t packetNumber)
{
	m_messageWriter.init(MessageType::UPDATE_DYNAMIC);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint64_t>(packetNumber);
	m_messageWriter.write<float>(x);
	m_messageWriter.write<float>(y);
	auto input = entity.component<InputComponent>();
	if(input.valid())
	{
		m_messageWriter.write<float>(input->moveX);
		m_messageWriter.write<float>(input->moveY);
	}
	return m_messageWriter.createPacket(0);
}

void NetServer::sendBombEntities(ENetPeer *peer)
{
	ComponentHandle<BombComponent> bomb;
	ComponentHandle<OwnerComponent> owner;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(bomb, owner, cell))
		send(peer, NetChannel::WORLD_RELIABLE, createBombPacket(entity, cell->x, cell->y, owner->entity));
}

ENetPacket *NetServer::createBombPacket(Entity entity, uint8_t x, uint8_t y, Entity owner)
{
	m_messageWriter.init(MessageType::CREATE_BOMB);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	m_messageWriter.write<uint64_t>(owner.id().id());
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::sendExplosionEntities(ENetPeer* peer)
{
	ComponentHandle<ExplosionComponent> explosion;
	ComponentHandle<SpreadComponent> spread;
	ComponentHandle<OwnerComponent> owner;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(explosion, spread, owner, cell))
		send(peer, NetChannel::WORLD_RELIABLE, createExplosionPacket(entity, cell->x, cell->y, spread->direction, spread->range, spread->spreadTime));
}

ENetPacket *NetServer::createExplosionPacket(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime)
{
	m_messageWriter.init(MessageType::CREATE_EXPLOSION);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	m_messageWriter.write<Direction>(direction);
	m_messageWriter.write<uint8_t>(range);
	m_messageWriter.write<float>(spreadTime);
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::sendPortalEntities(ENetPeer* peer)
{
	ComponentHandle<PortalComponent> portal;
	ComponentHandle<OwnerComponent> owner;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(portal, owner, cell))
		send(peer, NetChannel::WORLD_RELIABLE, createPortalPacket(entity, cell->x, cell->y, owner->entity));
}

ENetPacket *NetServer::createPortalPacket(Entity entity, uint8_t x, uint8_t y, Entity owner)
{
	m_messageWriter.init(MessageType::CREATE_PORTAL);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	m_messageWriter.write<uint64_t>(owner.id().id());
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::sendItemEntities(ENetPeer* peer)
{
	ComponentHandle<ItemComponent> item;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(item, cell))
		send(peer, NetChannel::WORLD_RELIABLE, createItemPacket(entity, cell->x, cell->y, item->type));
}

ENetPacket* NetServer::createItemPacket(Entity entity, uint8_t x, uint8_t y, ItemType type)
{
	m_messageWriter.init(MessageType::CREATE_ITEM);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	m_messageWriter.write<ItemType>(type);
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

ENetPacket* NetServer::createBoostEffectPacket(Entity entity, uint8_t x, uint8_t y, entityx::Entity target)
{
	m_messageWriter.init(MessageType::CREATE_BOOST_EFFECT);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	m_messageWriter.write<uint64_t>(target.id().id());
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::sendSmokeEntities(ENetPeer* peer)
{
}

ENetPacket* NetServer::createSmokePacket(Entity entity, uint8_t x, uint8_t y)
{
	m_messageWriter.init(MessageType::CREATE_SMOKE);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<uint8_t>(x);
	m_messageWriter.write<uint8_t>(y);
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::onChatMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	string msg = reader.read<string>();
	GameGlobals::events->emit<ChatEvent>(msg, info->name);

	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(msg);
	m_messageWriter.write<string>(info->name);
	broadcast(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}