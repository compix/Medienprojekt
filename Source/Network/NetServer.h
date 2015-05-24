#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include "../Events/SendChatEvent.h"
#include <entityx/entityx.h>

using NetCode::ServerConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using entityx::Receiver;

class NetServer : public Receiver<NetServer>
{
public:
	NetServer();

	void update();
	bool connect();
	void disconnect();

	void receive(const SendChatEvent &evt);

private:
	ServerConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
};
