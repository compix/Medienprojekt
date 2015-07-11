#include "MenuPageCreateGame.h"
#include "../../Events/CreateGameEvent.h"
#include <format.h>
#include "../../GameGlobals.h"
#include "../Menu.h"

MenuPageCreateGame::MenuPageCreateGame(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Create Game");

	auto width2 = 250;
	auto width3 = 250;
	auto height2 = 40;
	auto x1 = 50;
	auto x2 = 210;
	auto x3 = x2 + width2 + 20;
	auto y = 120;
	auto stepY = 50;
	auto labelYOffset = 6;

	createLabel(x1, y + labelYOffset, "Size: ");
	m_width = createSizeComboBox(x2, y, width2 * 0.4, height2, 3, 50, 21);
	createLabel(x2 + 117, y + labelYOffset + 5, "x");
	m_height = createSizeComboBox(x2 + width2 * 0.6, y, width2 * 0.4, height2, 3, 50, 21);

	m_playerCount = createComboBox(x3, y, width3, height2);
	m_playerCount->addItem("2 Players", 2);
	m_playerCount->addItem("3 Players", 3);
	m_playerCount->addItem("4 Players", 4);
	m_playerCount->setSelectedItem(2);
	m_playerCount->bindCallback(&MenuPageCreateGame::onChange, this, tgui::ComboBox::ItemSelected);

	y += stepY * 1.5f;

	createLabel(x1, y + labelYOffset, "#");
	createLabel(x2, y + labelYOffset, "Name");
	createLabel(x3, y + labelYOffset, "Type");
	y += stepY;
	for (int i = 0; i < 4; i++)
	{
		createLabel(x1, y + labelYOffset, fmt::format("Player {}:", i+1));
		m_name[i] = createEditBox(x2, y, width2, height2);
		m_name[i]->setText(fmt::format("Player {}", i + 1));
		if (i > 0)
		{
			m_type[i] = createComboBox(x3, y, width3, height2);
			m_type[i]->addItem("Local Player", (int)CreateGamePlayerType::LOCAL);
			m_type[i]->addItem("Computer", (int)CreateGamePlayerType::COMPUTER);
			m_type[i]->addItem("Network Slot", (int)CreateGamePlayerType::CLIENT);
			m_type[i]->setSelectedItem((int)CreateGamePlayerType::LOCAL);
			m_type[i]->bindCallback(&MenuPageCreateGame::onChange, this, tgui::ComboBox::ItemSelected);
		}
		y += stepY;
	}

	y += stepY * 0.25f;
	createLabel(x1, y + labelYOffset, "IP: Port");
	m_host = createEditBox(x2, y, width2, height2);
	m_host->setText("");
	m_port = createEditBox(x3, y, width3, height2);
	m_port->setText("1234");

	y += stepY * 1.25f;

	m_submitButton = createButton(x2, y, width2, height2, "Start");
	m_submitButton->bindCallback(&MenuPageCreateGame::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name[0].get();
	onChange();
}

void MenuPageCreateGame::onSubmit()
{
	int numPlayers = m_playerCount->getSelectedItemId();
	int onlinePlayers = 0;
	std::vector<CreateGamePlayerInfo> players;
	for (int i = 0; i < numPlayers; i++)
	{
		auto name = m_name[i]->getText().toAnsiString();
		auto type = i == 0 ? CreateGamePlayerType::LOCAL : (CreateGamePlayerType)m_type[i]->getSelectedItemId();
		if (type == CreateGamePlayerType::CLIENT)
			onlinePlayers++;
		players.push_back(CreateGamePlayerInfo(name, type));
	}

	int width = m_width->getSelectedItemId();
	int height = m_height->getSelectedItemId();

	if (onlinePlayers)
	{
		std::string host = m_host->getText();
		std::string port = m_port->getText();
		int portValue = atoi(port.c_str());

		GameGlobals::events->emit<CreateGameEvent>(width, height, players, host, portValue, onlinePlayers);
		//fixme: push lobby page
	}
	else
	{
		GameGlobals::events->emit<CreateGameEvent>(width, height, players);
	}
	m_menu.popPage();
	m_menu.popPage();
}

void MenuPageCreateGame::setEnabled(tgui::Widget::Ptr widget, bool enable)
{
	if (enable)
	{
		widget->enable();
		widget->setTransparency(255);
	}
	else
	{
		widget->disable();
		widget->setTransparency(128);
	}
}

void MenuPageCreateGame::setEnabledComboBox(tgui::ComboBox::Ptr widget, bool enable)
{
	setEnabled(widget, enable);
	sf::Uint8 a = enable ? 255 : 128;
	sf::Color bg = widget->getBackgroundColor();
	sf::Color border = widget->getBorderColor();
	bg.a = a;
	border.a = a;
	widget->setBackgroundColor(bg);
	widget->setBorderColor(border);
}

tgui::ComboBox::Ptr MenuPageCreateGame::createSizeComboBox(float x, float y, float width, float height, int from, int to, int selected)
{
	tgui::ComboBox::Ptr combo = createComboBox(x, y, width, height);
	for (int i = from; i<=to; i++)
		combo->addItem(fmt::format("{}", i), i);
	combo->setSelectedItem(selected - from);
	combo->setItemsToDisplay(8);
	return combo;
}

void MenuPageCreateGame::onChange()
{
	int numPlayers = m_playerCount->getSelectedItemId();
	bool online = false;
	for (int i = 1; i < 4; i++)
	{
		auto type = (CreateGamePlayerType)m_type[i]->getSelectedItemId();
		bool isClient = type == CreateGamePlayerType::CLIENT;
		setEnabled(m_name[i], i < numPlayers && !isClient);
		if (isClient)
			online = true;
		setEnabledComboBox(m_type[i], i < numPlayers);
	}

	setEnabled(m_host, online);
	setEnabled(m_port, online);
}
