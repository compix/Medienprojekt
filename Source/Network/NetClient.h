#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include "../Events/SendChatEvent.h"
#include <entityx/entityx.h>

using NetCode::ClientConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using entityx::EventManager;
using entityx::Receiver;

class NetClient : public Receiver<NetClient>
{
public:
	NetClient(EventManager &events);

	void update();
	bool connect();
	void disconnect();

	void receive(const SendChatEvent &evt);

private:
	EventManager &m_events;
	ClientConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
};
