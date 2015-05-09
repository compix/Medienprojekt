#pragma once
#include <chrono>
#include <iostream>
#include "Message.h"

namespace NetCode
{
	inline bool init()
	{
		atexit(enet_deinitialize);
		return enet_initialize() == 0;
	}

	inline int64_t getTime()
	{
		using namespace std::chrono;
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		return ms.count();
	}

	enum class ConnectionState : uint8_t
	{
		DISCONNECTED,
		CONNECTING,
		CONNECTED,
		DISCONNECTING
	};

	template <typename T>
	class Connection
	{
	public:
		Connection() :m_host(nullptr), m_handler(nullptr) {}
		virtual ~Connection() { disconnectNow();}

		bool isConnected() { return m_state == ConnectionState::CONNECTED; }
		bool isDisconnected() { return m_state == ConnectionState::DISCONNECTED; }

		void disconnect()
		{
			if (m_host && m_state == ConnectionState::CONNECTED)
			{
				for (size_t i = 0; i < m_host->peerCount; i++)
					enet_peer_disconnect(&m_host->peers[i], 0);
				m_state = ConnectionState::DISCONNECTING;
			}
		}

		void disconnectNow()
		{
			if (m_host && m_state == ConnectionState::CONNECTED)
			{
				for (size_t i = 0; i < m_host->peerCount; i++)
					enet_peer_disconnect_now(&m_host->peers[i], 0);
				disconnectInternal();
			}
		}

		void disconnectLater()
		{
			if (m_host && m_state == ConnectionState::CONNECTED)
			{
				for (size_t i = 0; i < m_host->peerCount; i++)
					enet_peer_disconnect_later(&m_host->peers[i], 0);
				m_state = ConnectionState::DISCONNECTING;
			}
		}

		ENetHost* getHost() { return m_host; }
		virtual void setHandler(MessageHandler<T> *handler) { m_handler = handler; }

		virtual void update()
		{
			if (m_state != ConnectionState::DISCONNECTED)
			{
				int serviceResult = 1;

				/* Keep doing host_service until no events are left */
				while ((serviceResult = enet_host_service(m_host, &m_event, 0)) > 0)
				{
					switch (m_event.type)
					{
					case ENET_EVENT_TYPE_CONNECT: onConnect(m_event); break;
					case ENET_EVENT_TYPE_RECEIVE: onReceive(m_event); break;
					case ENET_EVENT_TYPE_DISCONNECT: onDisconnect(m_event); break;
					}
				}

				if (serviceResult < 0)
				{
					cerr << "Error during host service" << endl;
					exit(EXIT_FAILURE);
				}
			}
		}


		using EventCallback = function<void(ENetEvent &event)>;
		virtual void setConnectCallback(EventCallback callback) { m_connectCallback = callback; }
		virtual void setDisconnectCallback(EventCallback callback) { m_disconnectCallback = callback; }
		
	protected:
		ENetHost *m_host;
		MessageHandler<T> *m_handler;
		ENetEvent m_event;
		ConnectionState m_state = ConnectionState::DISCONNECTED;
		EventCallback m_connectCallback;
		EventCallback m_disconnectCallback;


		virtual void disconnectInternal()
		{
			enet_host_destroy(m_host);
			m_host = nullptr;
			m_state = ConnectionState::DISCONNECTED;
		}

		void onConnect(ENetEvent &event)
		{
			if (m_state == ConnectionState::CONNECTING)
			{
				m_state = ConnectionState::CONNECTED;
			}
			else if (m_state == ConnectionState::CONNECTED)
			{
			}

			// Notify listener
			if (m_connectCallback)
				m_connectCallback(event);

			// fixme: if disconnecting, disconnect this one ?
		}

		void onDisconnect(ENetEvent &event)
		{
			if (m_state == ConnectionState::DISCONNECTING && !m_host->connectedPeers)
				disconnectInternal();

			// Notify listener
			if (m_disconnectCallback)
				m_disconnectCallback(event);
		}

		void onReceive(ENetEvent &evt)
		{
			if (m_state == ConnectionState::CONNECTED && m_handler)
				m_handler->handleMessage(evt);
			enet_packet_destroy(evt.packet);
		}
	};

	template <typename T>
	class ServerConnection : public Connection<T>
	{
	public:
		bool connect(const char *hostname, int port, int clients, int channels)
		{
			if (this->m_state != ConnectionState::DISCONNECTED)
				this->disconnectNow();

			ENetAddress address;
			if (!hostname)
				address.host = ENET_HOST_ANY;
			else
				enet_address_set_host(&address, hostname);
			address.port = port;

			this->m_host = enet_host_create(&address, clients, channels, 0, 0);
			if (this->m_host == nullptr)
				return false;

			this->m_state = ConnectionState::CONNECTED;
			return true;
		}
	};

	template <typename T>
	class ClientConnection : public Connection<T>
	{
	public:
		ClientConnection() : m_peer(nullptr) {}

		ENetPeer *getPeer() { return m_peer; }

		bool connect(const char *hostname, int port, int channels)
		{
			if (this->m_state != ConnectionState::DISCONNECTED)
				this->disconnectNow();

			this->m_host = enet_host_create(nullptr, 1, channels, 0, 0);
			if (this->m_host == nullptr)
				return false;

			ENetAddress address;
			enet_address_set_host(&address, hostname);
			address.port = port;

			m_peer = enet_host_connect(this->m_host, &address, channels, 0);
			if (m_peer == nullptr)
			{
				this->disconnectNow();
				return false;
			}

			this->m_state = ConnectionState::CONNECTING;
			m_connectionTime = getTime();
			return true;
		}

		void disconnectInternal() override
		{
			Connection<T>::disconnectInternal();
			m_peer = nullptr;
		}

		void update() override
		{
			Connection<T>::update();
			if (this->m_state == ConnectionState::CONNECTING && (getTime() - m_connectionTime) > 5000)
			{
				this->disconnectNow();
				//fixme: notify listeners if not done automaticly.
			}
		}

	protected:
		ENetPeer *m_peer;
		uint64_t m_connectionTime;
	};
}
