#ifndef _FAKE_CHESS_APPLICATION_HPP_
#define _FAKE_CHESS_APPLICATION_HPP_

#include <memory>
#include <string>


namespace sf { class RenderWindow; }

class Game;


class Application final
{
	bool m_bLaunched = true;

	std::unique_ptr<sf::RenderWindow> m_pWindow;
	std::unique_ptr<Game> m_pGame;

public:
	Application();
	~Application();

	void init();

	void update();

	void destroy();

private:
	void createWindow();

};

#endif
