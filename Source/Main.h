#pragma once



#include "Network/NetServer.h"
#include "Network/NetClient.h"
#include "LayerManager.h"



using std::unique_ptr;

class Main
{
public:
	~Main() { disconnect(); }
	int run();
	void disconnect();

private:
	void onExit()
	{
		m_running = false;
	}
	void onCreateGame(uint8_t width, uint8_t height, const vector<CreateGamePlayerInfo> &players, int port, int maxClients);
	void onJoinGame(const string &host, int port, const string &name);
	void onForceDisconnect(const string &message);
	void onPreload(int progress, int total, string nextSection, const string &nextFilename);

private:
	bool m_running = true;
	unique_ptr<NetClient> m_client;
	unique_ptr<NetServer> m_server;
	bool m_forceDisconnect = false;
};
