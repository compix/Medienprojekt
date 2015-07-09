#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include <entityx/entityx.h>
#include "../Events/ExplosionCreatedEvent.h"

struct PortalCreatedEvent;
struct SendChatEvent;
struct BombCreatedEvent;
using NetCode::ServerConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using entityx::Receiver;
using entityx::EntityManager;
using entityx::EntityDestroyedEvent;

class NetServer : public Receiver<NetServer>
{
public:
	NetServer();
	~NetServer();

	void update();
	bool connect(const std::string &hostname, int port);
	void disconnect();

	void receive(const SendChatEvent &evt);
	void receive(const BombCreatedEvent &evt);
	void receive(const ExplosionCreatedEvent& evt);
	void receive(const EntityDestroyedEvent &evt);
	void receive(const PortalCreatedEvent &evt);
	void onHandshakeMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onInputDirectionMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onInputBombActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onInputSkillActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);

private:
	template <class T>
	void sendBlockEntities(ENetPeer* peer, MessageType type);
	void sendPlayerEntities(ENetPeer* peer);
	ENetPacket* createPlayerPacket(entityx::Entity entity, float x, float y);
	void broadcastDynamicUpdates();
	ENetPacket* createUpdateDynamicPacket(entityx::Entity entity, float x, float y);
	void sendBombEntities(ENetPeer* peer);
	ENetPacket* createBombPacket(entityx::Entity entity, uint8_t x, uint8_t y, entityx::Entity owner);
	ENetPacket* createExplosionPacket(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime);
	void sendPortalEntities(ENetPeer* peer);
	ENetPacket* createPortalPacket(Entity entity, uint8_t x, uint8_t y, Entity owner);
	void broadcast(NetChannel channel, ENetPacket *packet);
	void send(ENetPeer* peer, NetChannel channel, ENetPacket *packet);

private:
	ServerConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
};
