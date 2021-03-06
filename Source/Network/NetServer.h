#pragma once
#include "NetConstants.h"
#include "../NetCode/Message.h"
#include "../NetCode/Connection.h"
#include <entityx/entityx.h>
#include "../Events/ExplosionCreatedEvent.h"
#include "NetPlayerInfo.h"
#include "../GameConstants.h"
#include "../Events/LavaSpotMarkedEvent.h"
#include "../Components/JumpComponent.h"

struct JumpEvent;
struct LavaCreatedEvent;
struct Phase2StartedEvent;
struct StartGameEvent;
struct HoldingStatusEvent;
struct SkillEvent;
enum class BombType : uint8_t;
struct ResetGameEvent;
struct GameOverEvent;
struct SetReadyEvent;
struct DeathEvent;
struct BoostEffectCreatedEvent;
struct ItemCreatedEvent;
struct SoundEvent;
enum class ItemType : uint8_t;
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

class DynamicUpdateWriter
{
public:
	DynamicUpdateWriter();
	ENetPacket *addEntity(Entity &entity, float x, float y, float velX, float velY, uint64_t packetNumber);
	ENetPacket *finish();

private:
	MessageWriter<MessageType> m_messageWriter;
};

class NetServer : public Receiver<NetServer>
{
public:
	NetServer();
	~NetServer();

	void update(float deltaTime);
	bool connect(const CreateGameEvent& evt);
	void disconnect();

	void receive(const SendChatEvent &evt);
	void receive(const BombCreatedEvent &evt);
	void receive(const ExplosionCreatedEvent& evt);
	void receive(const EntityDestroyedEvent &evt);
	void receive(const PortalCreatedEvent &evt);
	void receive(const ItemCreatedEvent &evt);
	void receive(const BoostEffectCreatedEvent &evt);
	void receive(const DeathEvent &evt);
	void receive(const SetReadyEvent &evt);
	void receive(const GameOverEvent &evt);
	void receive(const ResetGameEvent &evt);
	void receive(const StartGameEvent& evt);
	void receive(const SkillEvent &evt);
	void receive(const HoldingStatusEvent &evt);
	void receive(const Phase2StartedEvent& event);
	void receive(const LavaSpotMarkedEvent& event);
	void receive(const LavaCreatedEvent& event);
	void receive(const SoundEvent& event);
	void receive(const JumpEvent& event);

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
	ENetPacket* createPlayerPacket(entityx::Entity entity, float x, float y, uint8_t playerIndex);
	void broadcastDynamicUpdates();
	void sendBombEntities(ENetPeer* peer);
	ENetPacket* createBombPacket(entityx::Entity entity, uint8_t x, uint8_t y, entityx::Entity owner, BombType type);
	void sendExplosionEntities(ENetPeer* peer);
	ENetPacket* createExplosionPacket(Entity entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime, BombType bombType);
	void sendPortalEntities(ENetPeer* peer);
	ENetPacket* createPortalPacket(Entity entity, uint8_t x, uint8_t y, Entity owner);
	void sendItemEntities(ENetPeer* peer);
	ENetPacket* createItemPacket(Entity entity, uint8_t x, uint8_t y, ItemType type);
	ENetPacket* createBoostEffectPacket(Entity entity, uint8_t x, uint8_t y, entityx::Entity target);
	void broadcast(NetChannel channel, ENetPacket *packet);
	void send(ENetPeer* peer, NetChannel channel, ENetPacket *packet);
	void sendStartGame(NetPlayerInfo* netPlayerInfo);
	Entity getFreeSlotEntity();
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
	DynamicUpdateWriter m_dynamicUpdateWriter;
	uint8_t m_width;
	uint8_t m_height;
	float m_countdown = 0;
	float m_nextBroadcast = 0;
};
