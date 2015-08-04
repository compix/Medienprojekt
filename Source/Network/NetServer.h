#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"

#include "NetPlayerInfo.h"
#include "../GameConstants.h"
#include <signal11/Signal.h>

enum class ItemType : uint8_t;
using NetCode::ServerConnection;
using NetCode::MessageHandler;
using NetCode::MessageWriter;
using NetCode::MessageReader;
using std::vector;
using std::string;
struct CreateGamePlayerInfo;
enum class Direction : uint8_t;

namespace ECS {
	class Engine;
};
using ECS::Engine;

class NetServer 
{
public:
	NetServer();
	~NetServer();

	void update(float deltaTime);
	bool connect(uint8_t width, uint8_t height, const vector<CreateGamePlayerInfo> &players, int port, int maxClients);
	void disconnect();

	void onSendChat(const string &message);
	void onBombCreated(Entity *entity, uint8_t x, uint8_t y, Entity *owner);
	void onExplosionCreated(Entity *entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime);
	void onEntityDestroyed(Entity *entity);
	void onPortalCreated(Entity *entity, uint8_t x, uint8_t y, Entity *owner);
	void onItemCreated(Entity *entity, uint8_t x, uint8_t y, ItemType type);
	void onBoostEffectCreated(Entity *entity, uint8_t x, uint8_t y, Entity *target);
	void onSmokeCreated(Entity *entity, uint8_t x, uint8_t y);
	void onDeath(Entity *dyingEntity);
	void onSetReady(int playerIndex, bool ready);

	void onHandshakeMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onInputDirectionMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onInputBombActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onInputSkillActivatedMessage(MessageReader<MessageType>& reader, ENetEvent& evt);
	void onChatMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void onPlayerReadyMessage(MessageReader<MessageType> &reader, ENetEvent &evt);
	void broadcastPlayerReady(uint8_t playerIndex, bool ready);

private:
	template <class T>
	void sendBlockEntities(ENetPeer* peer, MessageType type);
	void sendPlayerEntities(ENetPeer* peer);
	ENetPacket* createPlayerPacket(Entity *entity, float x, float y, uint8_t playerIndex);
	void broadcastDynamicUpdates();
	ENetPacket* createUpdateDynamicPacket(Entity *entity, float x, float y, uint64_t packetNumber);
	void sendBombEntities(ENetPeer* peer);
	ENetPacket* createBombPacket(Entity *entity, uint8_t x, uint8_t y, Entity *owner);
	void sendExplosionEntities(ENetPeer* peer);
	ENetPacket* createExplosionPacket(Entity *entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime);
	void sendPortalEntities(ENetPeer* peer);
	ENetPacket* createPortalPacket(Entity *entity, uint8_t x, uint8_t y, Entity *owner);
	void sendItemEntities(ENetPeer* peer);
	ENetPacket* createItemPacket(Entity *entity, uint8_t x, uint8_t y, ItemType type);
	ENetPacket* createBoostEffectPacket(Entity *entity, uint8_t x, uint8_t y, Entity *target);
	void sendSmokeEntities(ENetPeer* peer);
	ENetPacket* createSmokePacket(Entity *entity, uint8_t x, uint8_t y);
	void broadcast(NetChannel channel, ENetPacket *packet);
	void send(ENetPeer* peer, NetChannel channel, ENetPacket *packet);
	void sendStartGame(NetPlayerInfo* netPlayerInfo);
	Entity *getFreeSlotEntity();
	void emitLobbyEvent(bool disable);
	bool allPlayersReady();
	void forceReady();
	void makeUniqueName(NetPlayerInfo* info);
	void startCountdown();
	void startGame();

private:
	ServerStatus m_status = ServerStatus::INIT;
	NetPlayerInfo m_playerInfos[GameConstants::MAX_PLAYERS];
	uint8_t m_numPlayers = 0;
	ServerConnection<MessageType> m_connection;
	MessageHandler<MessageType> m_handler;
	MessageWriter<MessageType> m_messageWriter;
	uint8_t m_width;
	uint8_t m_height;
	float m_countdown = 0;
	float m_nextBroadcast = 0;
	ConnectionScope m_connections;
	const std::vector<Entity *> *m_dynamicEntities;
};
