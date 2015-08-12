#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include <entityx/entityx.h>
#include <map>

using NetCode::ClientConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using entityx::EventManager;
using entityx::Receiver;
using entityx::Entity;

struct SendChatEvent;
struct SetReadyEvent;

class NetClient : public Receiver<NetClient>
{
public:
	NetClient();
	~NetClient();

	void update(float deltaTime);
	void sendInputEventMessage(MessageType type);
	bool connect(const std::string &hostname, int port);
	void disconnect();

	void receive(const SendChatEvent &evt);
	void receive(const SetReadyEvent &evt);

	void onHandshakeMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onPlayerReadyMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onStartGameMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onPlayerJoinedMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onCreateSolidBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBlockMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateFloorMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreatePlayerMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBombMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateExplosionMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreatePortalMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateItemMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateBoostEffectMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCreateSmokeMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onDeathMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onDestroyEntityMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onUpdateDynamicMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onCountdownMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onAllReadyMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onGameOverMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onResetGameMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
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
	uint8_t m_playerIndex = 0;
	float m_nextSend = 0;
};
