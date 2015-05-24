#include "NetClient.h"
#include "NetConstants.h"
#include "../Events/ChatEvent.h"
#include <iostream>
#include "../GameGlobals.h"
#include "../Events/PlayerJoinEvent.h"
#include "../Game.h"

using namespace std;
using namespace NetCode;

NetClient::NetClient()
	: m_messageWriter(1024)
{
	m_handler.setCallback(MessageType::HANDSHAKE, &NetClient::onHandshakeMessage, this);
	m_handler.setCallback(MessageType::CHAT, &NetClient::onChatMessage, this);
	m_handler.setCallback(MessageType::PLAYER_JOINED, &NetClient::onPlayerJoinedMessage, this);
	m_handler.setCallback(MessageType::CREATE_SOLID_BLOCK, &NetClient::onCreateSolidBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_BLOCK, &NetClient::onCreateBlockMessage, this);
	m_handler.setCallback(MessageType::CREATE_FLOOR, &NetClient::onCreateFloorMessage, this);
	
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([this](ENetEvent &event)
	{
		cout << "Connected to server!" << endl;

		m_messageWriter.init(MessageType::HANDSHAKE);
		m_messageWriter.write<string>("Santo");
		ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(m_connection.getPeer(), (enet_uint8)NetChannel::HANDSHAKE, packet);
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "Disconnected from server!" << endl;
	});
}

NetClient::~NetClient()
{
	m_connection.disconnectNow();
	cout << "Connection closed" << endl;
}

void NetClient::update()
{
	m_connection.update();
}

bool NetClient::connect()
{
	return m_connection.connect("localhost", NetConstants::DEFAULT_PORT, (int)NetChannel::COUNT);
}

void NetClient::disconnect()
{
	m_connection.disconnect();
}

void NetClient::receive(const SendChatEvent& evt)
{
	m_messageWriter.init(MessageType::CHAT);
	m_messageWriter.write<string>(evt.message);
	ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_connection.getPeer(), (enet_uint8)NetChannel::CHAT, packet);
}

void NetClient::onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t width = reader.read<uint8_t>();
	uint8_t height = reader.read<uint8_t>();
	GameGlobals::game->init(width, height);
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
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	GameGlobals::entityFactory->createSolidBlock(y, x);
}

void NetClient::onCreateBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	GameGlobals::entityFactory->createBlock(y, x);
}

void NetClient::onCreateFloorMessage(MessageReader<MessageType>& reader, ENetEvent& evt)
{
	uint8_t x = reader.read<uint8_t>();
	uint8_t y = reader.read<uint8_t>();
	GameGlobals::entityFactory->createFloor(y, x);
}
