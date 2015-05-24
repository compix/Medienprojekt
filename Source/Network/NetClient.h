#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include "../Events/SendChatEvent.h"
#include <entityx/entityx.h>

using NetCode::ClientConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using entityx::EventManager;
using entityx::Receiver;

class NetClient : public Receiver<NetClient>
{
public:
	NetClient();
	~NetClient();

	void update();
	bool connect();
	void disconnect();

	void receive(const SendChatEvent &evt);
	void onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onPlayerJoinedMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onCreateSolidBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateFloorMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
private:
	ClientConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
};
