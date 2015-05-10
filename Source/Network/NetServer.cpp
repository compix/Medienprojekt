#include "NetServer.h"
#include <iostream>
#include "NetConstants.h"
#include "../Events/ChatEvent.h"

using namespace std;
using namespace NetCode;

NetServer::NetServer(EventManager &events)
	: m_messageWriter(1024), m_events(events)
{
	m_handler.setCallback(MessageType::CHAT, [this](MessageReader<MessageType> &reader, ENetEvent &event)
	{
		string msg = reader.read<string>();
		string name = (const char *)event.peer->data;
		m_events.emit<ChatEvent>(msg, name);

		m_messageWriter.init(MessageType::CHAT);
		m_messageWriter.write<string>(msg);
		m_messageWriter.write<string>(name);
		ENetPacket *packet = m_messageWriter.createPacket(ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(m_connection.getHost(), (enet_uint8) NetChannel::CHAT, packet);
	});
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([](ENetEvent &event)
	{
		cout << "A client connected!" << endl;
		event.peer->data = (void*) "Client";
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "A client disconnected!" << endl;
		event.peer->data = nullptr;
	});
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
	m_events.emit<ChatEvent>(evt.message, "Server");
}
