#pragma once
#include <entityx/entityx.h>
#include "Events/ExitEvent.h"
#include "Events/CreateLocalGameEvent.h"
#include "Events/CreateServerEvent.h"
#include "Events/JoinServerEvent.h"
#include "Network/NetServer.h"
#include "Network/NetClient.h"
#include "LayerManager.h"

using entityx::Receiver;
using std::unique_ptr;

class Main : public Receiver<Main>
{
public:
	int run();

	void receive(const ExitEvent& evt)
	{
		m_running = false;
	}
	void receive(const CreateLocalGameEvent& evt);
	void receive(const CreateServerEvent& evt);
	void receive(const JoinServerEvent& evt);
	void disconnect();

private:
	bool m_running = true;
	unique_ptr<NetClient> m_client;
	unique_ptr<NetServer> m_server;
};
