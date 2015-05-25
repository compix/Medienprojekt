#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include "../Events/SendChatEvent.h"
#include <entityx/entityx.h>
#include <map>

using NetCode::ClientConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using entityx::EventManager;
using entityx::Receiver;
using entityx::Entity;

class NetClient : public Receiver<NetClient>
{
public:
	NetClient();
	~NetClient();

	void update();
	void sendInputEventMessage(MessageType type);
	bool connect();
	void disconnect();

	void receive(const SendChatEvent &evt);
	void onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onPlayerIdMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onPlayerJoinedMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onCreateSolidBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateFloorMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreatePlayerMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBombMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateExplosionMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onDestroyEntityMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onUpdatePlayerMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
private:
	void mapEntity(uint64_t id, Entity entity);
	Entity getEntity(uint64_t id);
	void send(NetChannel channel, ENetPacket *packet);

private:
	ClientConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
	std::map<uint64_t, Entity> entityMap;
	Entity m_playerEntity;
};
