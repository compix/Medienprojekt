#include "NetClient.h"
#include "NetConstants.h"
#include "../Events/ChatEvent.h"
#include <iostream>

using namespace std;
using namespace NetCode;

NetClient::NetClient(EventManager &events)
	: m_messageWriter(1024), m_events(events)
{
	m_handler.setCallback(MessageType::CHAT, [this](MessageReader<MessageType> &reader, ENetEvent &event)
	{
		string msg = reader.read<string>();
		string name = reader.read<string>();
		m_events.emit<ChatEvent>(msg, name);
	});
	m_connection.setHandler(&m_handler);
	m_connection.setConnectCallback([](ENetEvent &event)
	{
		cout << "Connected to server!" << endl;
		event.peer->data = (void*) "Server";
	});
	m_connection.setDisconnectCallback([](ENetEvent &event)
	{
		cout << "Disconnected from server!" << endl;
		event.peer->data = nullptr;
	});
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
	enet_peer_send(m_connection.getPeer(), 0, packet);
}