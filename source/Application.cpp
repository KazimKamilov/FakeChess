#include "Application.hpp"
#include "Log.hpp"
#include "Game.hpp"

#include "SFML/Window.hpp"
#include "SFML/Graphics/RenderWindow.hpp"


Application::Application()
{
	this->createWindow();
}

Application::~Application()
{
	m_pWindow->close();
}

void Application::init()
{
	log::get() << "Creating game..." << endl;

	m_pGame = std::make_unique<Game>(m_pWindow.get());

	log::get() << "Done." << endl;
}

void Application::update()
{
	sf::Clock timer;

	while (m_bLaunched && m_pWindow->isOpen())
	{
		sf::Event event;

		while (m_pWindow->pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::EventType::Closed:
					return;

				case sf::Event::EventType::Resized:
					m_pGame->setViewSize({
						static_cast<int>(event.size.width),
						static_cast<int>(event.size.height)
					});
				break;

				default:
				break;
			}
		}

		const float delta = static_cast<float>(timer.restart().asMilliseconds()) / 10.0f;

		m_pGame->update(delta);

		m_pWindow->display();
	}
}

void Application::destroy()
{
	log::get() << "Application will be closed..." << endl;
}

void Application::createWindow()
{
	log::get() << "Creating window..." << endl;

	sf::ContextSettings context_settings;
	context_settings.depthBits = 24u;
	context_settings.stencilBits = 8u;
	context_settings.antialiasingLevel = 8u;
	context_settings.majorVersion = 2u;
	context_settings.minorVersion = 1u;

	m_pWindow = std::make_unique<sf::RenderWindow>();

	m_pWindow->create(sf::VideoMode(640, 680), "Fake Chess", sf::Style::Default, context_settings);
	m_pWindow->setFramerateLimit(75u);
	m_pWindow->setVerticalSyncEnabled(true);

	m_pWindow->setActive();

	log::get() << "Done." << endl;
}
