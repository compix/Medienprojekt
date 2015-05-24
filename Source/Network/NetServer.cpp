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
#include "../Components/SolidBlockComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/FloorComponent.h"
#include "../Components/BlockComponent.h"

using namespace std;
using namespace NetCode;

NetServer::NetServer()
	: m_messageWriter(1024)
{
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
	ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(m_connection.getHost(), (enet_uint8)NetChannel::CHAT, packet);
	GameGlobals::events->emit<ChatEvent>(evt.message, "Server");
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
	ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(evt.peer, (enet_uint8)NetChannel::HANDSHAKE, packet);

	// Send all blocks:
	sendBlockEntities<SolidBlockComponent>(evt.peer, MessageType::CREATE_SOLID_BLOCK);
	sendBlockEntities<FloorComponent>(evt.peer, MessageType::CREATE_FLOOR);
	sendBlockEntities<BlockComponent>(evt.peer, MessageType::CREATE_BLOCK);

//	m_messageWriter.init(MessageType::START_GAME);
//	packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
//	enet_peer_send(evt.peer, (enet_uint8)NetChannel::WORLD, packet);

	//Fixme: send entities to new player

	// Notify all playes about the join
	m_messageWriter.init(MessageType::PLAYER_JOINED);
	m_messageWriter.write<string>(info->name);
	packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(m_connection.getHost(), (enet_uint8)NetChannel::STATUS, packet);
}

template<typename T>
void NetServer::sendBlockEntities(ENetPeer *peer, MessageType type)
{
	ENetPacket *packet;
	ComponentHandle<T> block;
	ComponentHandle<CellComponent> cell;
	using GameGlobals::entities;
	for (Entity entity : entities->entities_with_components(block, cell))
	{
		m_messageWriter.init(type);
		m_messageWriter.write<uint8_t>(cell->x);
		m_messageWriter.write<uint8_t>(cell->y);
		packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, (enet_uint8)NetChannel::WORLD, packet);
	}
}

void NetServer::onChatMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	NetPlayerInfo *info = static_cast<NetPlayerInfo *>(evt.peer->data);
	string msg = reader.read<string>();
	GameGlobals::events->emit<ChatEvent>(msg, info->name);

	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(msg);
	m_messageWriter.write<string>(info->name);
	ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(m_connection.getHost(), (enet_uint8)NetChannel::CHAT, packet);
}