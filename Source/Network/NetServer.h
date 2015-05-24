#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include "../Events/SendChatEvent.h"
#include <entityx/entityx.h>

using NetCode::ServerConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using entityx::Receiver;
using entityx::EntityManager;

class NetServer : public Receiver<NetServer>
{
public:
	NetServer();
	~NetServer();

	void update();
	bool connect();
	void disconnect();

	void receive(const SendChatEvent &evt);
	void onHandshakeMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);

private:
	template <class T>
	void sendBlockEntities(ENetPeer* peer, MessageType type);

private:
	ServerConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
};
