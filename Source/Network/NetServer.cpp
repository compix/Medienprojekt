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

using namespace std;
using namespace NetCode;

NetServer::NetServer()
	: m_messageWriter(1024)
{
	GameGlobals::events->subscribe<SendChatEvent>(*this);
	GameGlobals::events->subscribe<BombCreatedEvent>(*this);
	GameGlobals::events->subscribe<EntityDestroyedEvent>(*this);
	m_handler.setCallback(MessageType::CHAT, &NetServer::onChatMessage, this);
	m_handler.setCallback(MessageType::HANDSHAKE, &NetServer::onHandshakeMessage, this);

	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([](ENetEvent &event)
	{
		cout << "A client connected!" << endl;
		event.peer->data = new NetPlayerInfo;
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		NetPlayerInfo *info = static_cast<NetPlayerInfo *>(event.peer->data);
		if (info->connecting)
			GameGlobals::events->emit<DisconnectEvent>("The client was unable to connect", info);
		else
			GameGlobals::events->emit<DisconnectEvent>("The client disconnected", info);
		cout << "A client disconnected!" << endl;
		event.peer->data = nullptr;
		delete info;
	});
}

NetServer::~NetServer()
{
	GameGlobals::events->unsubscribe<SendChatEvent>(*this);
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);

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

void NetServer::update()
{
	broadcastPlayerUpdates();
	m_connection.update();
}

bool NetServer::connect()
{
	return m_connection.connect(nullptr, NetConstants::DEFAULT_PORT, NetConstants::MAX_CLIENTS, (enet_uint8)NetChannel::COUNT);
}

void NetServer::disconnect()
{
	m_connection.disconnect();
}

void NetServer::receive(const SendChatEvent& evt)
{
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(evt.message);
	m_messageWriter.write<string>("Server");
	broadcast(NetChannel::CHAT, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	GameGlobals::events->emit<ChatEvent>(evt.message, "Server");
}

void NetServer::receive(const BombCreatedEvent& evt)
{
	broadcast(NetChannel::WORLD, createBombPacket(evt.entity, evt.x, evt.y, evt.owner));
}

void NetServer::receive(const EntityDestroyedEvent& evt)
{
	m_messageWriter.init(MessageType::DESTROY_ENTITY);
	m_messageWriter.write<uint64_t>(evt.entity.id().id());
	broadcast(NetChannel::WORLD, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
}

void NetServer::broadcast(NetChannel channel, ENetPacket *packet)
{
	enet_host_broadcast(m_connection.getHost(), (enet_uint8)channel, packet);
}

void NetServer::send(ENetPeer* peer, NetChannel channel, ENetPacket *packet)
{
	enet_peer_send(peer, (enet_uint8)channel, packet);
}

void NetServer::onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	info->name = reader.read<string>();
	GameGlobals::events->emit<PlayerJoinEvent>(info->name);

	// Send greeting back
	m_messageWriter.init(MessageType::HANDSHAKE);
	//fixme: world status, player id, corrected name, ...
	m_messageWriter.write<uint8_t>(GameGlobals::game->getWidth());
	m_messageWriter.write<uint8_t>(GameGlobals::game->getHeight());
	send(evt.peer, NetChannel::HANDSHAKE, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));

	// Send all blocks:
	sendBlockEntities<SolidBlockComponent>(evt.peer, MessageType::CREATE_SOLID_BLOCK);
	sendBlockEntities<FloorComponent>(evt.peer, MessageType::CREATE_FLOOR);
	sendBlockEntities<BlockComponent>(evt.peer, MessageType::CREATE_BLOCK);

	//Fixme: send entities to new player
	sendPlayerEntities(evt.peer);
	sendBombEntities(evt.peer);

//	m_messageWriter.init(MessageType::START_GAME);
//	packet = ;
//	send(evt.peer, NetChannel::WORLD, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));

	// Notify all playes about the join
	m_messageWriter.init(MessageType::PLAYER_JOINED);
	m_messageWriter.write<string>(info->name);
	broadcast(NetChannel::STATUS, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
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
		send(peer, NetChannel::WORLD, m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE));
	}
}

void NetServer::sendPlayerEntities(ENetPeer *peer)
{
	ComponentHandle<InputComponent> input;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(input, transform, cell))
		send(peer, NetChannel::WORLD, createPlayerPacket(entity, transform->x, transform->y));
}

ENetPacket *NetServer::createPlayerPacket(Entity entity, float x, float y)
{
	m_messageWriter.init(MessageType::CREATE_PLAYER);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<float>(x);
	m_messageWriter.write<float>(y);
	return m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
}

void NetServer::broadcastPlayerUpdates()
{
	ComponentHandle<InputComponent> input;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(input, transform, cell))
		broadcast(NetChannel::MOVEMENT, createPlayerUpdatePacket(entity, transform->x, transform->y));
}

ENetPacket *NetServer::createPlayerUpdatePacket(Entity entity, float x, float y)
{
	m_messageWriter.init(MessageType::UPDATE_PLAYER);
	m_messageWriter.write<uint64_t>(entity.id().id());
	m_messageWriter.write<float>(x);
	m_messageWriter.write<float>(y);
	return m_messageWriter.createPacket(0);
}

void NetServer::sendBombEntities(ENetPeer *peer)
{
	ComponentHandle<BombComponent> bomb;
	ComponentHandle<OwnerComponent> owner;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(bomb, owner, cell))
		send(peer, NetChannel::WORLD, createBombPacket(entity, cell->x, cell->y, owner->entity));
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