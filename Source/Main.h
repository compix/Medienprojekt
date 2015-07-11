#pragma once
#include <entityx/entityx.h>
#include "Events/ExitEvent.h"
#include "Events/CreateGameEvent.h"
#include "Events/JoinGameEvent.h"
#include "Network/NetServer.h"
#include "Network/NetClient.h"
#include "LayerManager.h"

using entityx::Receiver;
using std::unique_ptr;

class Main : public Receiver<Main>
{
public:
	~Main() { disconnect(); }
	int run();

	void receive(const ExitEvent& evt)
	{
		m_running = false;
	}
	void initPlayers(const std::vector<CreateGamePlayerInfo> &players);
	void receive(const CreateGameEvent& evt);
	void receive(const JoinGameEvent& evt);
	void disconnect();

private:
	bool m_running = true;
	unique_ptr<NetClient> m_client;
	unique_ptr<NetServer> m_server;
	EventManager m_events;
};
