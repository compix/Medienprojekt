#include "MenuPageJoinServer.h"

MenuPageJoinServer::MenuPageJoinServer(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Join Server");
}
