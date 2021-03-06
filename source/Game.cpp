#include "Game.hpp"
#include "Log.hpp"
#include "Field.hpp"

#include "FieldCell.hpp"
#include "FieldFigure.hpp"

#include "AIPlayer.hpp"

#include "SFML/OpenGL.hpp"
#include "SFML/Graphics.hpp"

// DAMN WINDOWS DEFINES!
#ifdef ERROR
#undef ERROR
#endif

Game::Game(sf::RenderWindow* window) :
	m_pWindow {window},
	m_pFont {std::make_unique<sf::Font>()},
	m_pText {std::make_unique<sf::Text>()}
{
	if (!m_pFont->loadFromFile("terminus.ttf"))
		log::get(log::type::ERROR) << "Cannot load font!" << endl;

	m_pText->setFont(*m_pFont);

	m_pText->setCharacterSize(28u);
	m_pText->setPosition({Field::BoardStep, 590.0f});
	m_pText->setFillColor({255, 175, 64, 255});

	///////////////////////////////////////////////////////

	m_pField = std::make_unique<Field>(m_pWindow);

	m_pField->createCells();
	m_pField->createFigures();
	m_pField->createWinningCells();

	m_pAIPlayer = std::make_unique<AIPlayer>(this, !m_bPlayerWhite);

	this->waitForStart();
}

Game::~Game()
{
}

void Game::update(const float delta)
{
	// background
	m_pWindow->clear({8, 42, 22, 255});

	// Checking of winning conditions
	this->checkAndEndGameIfWin();

	// Our states
	switch (m_State)
	{
		case State::AI_THINK:
			this->processAI();
		break;

		case State::PLAYER_THINK:
			this->processPlayer();
		break;

		default:
		break;
	}

	m_pWindow->draw(*m_pText);

	m_pField->update(delta);

	m_pAIPlayer->drawPaths();
}

void Game::waitForStart()
{
	if (m_bPlayerWhite)
		this->setState(State::PLAYER_THINK);
	else
		this->setState(State::AI_THINK);
}

void Game::checkAndEndGameIfWin()
{
	if (const auto& win_pair = m_pField->getWinnerTeam();
		m_State != State::HAVE_WINNER &&
		(win_pair.first || win_pair.second))
	{
		if (win_pair.first && !win_pair.second)
			this->messageText(L"Игра завершена!\nРезультат: Белые фигуры выйграли!");
		else if (!win_pair.first && win_pair.second)
			this->messageText(L"Игра завершена!\nРезультат: Чёрные фигуры выйграли!");

		this->setState(State::HAVE_WINNER);
		m_pField->selectAllCells();
	}
}

void Game::processPlayer()
{
	this->messageText(L"Ход игрока...");

	this->processPlayerSelect();

	this->processPlayerMove();
}

void Game::processPlayerSelect()
{
	const auto& cells = m_pField->getCells();

	for (size_t i = 0u; i < cells.size(); ++i)
	{
		const auto& cell = cells[i];

		if (auto figure = cell->getFigureInside(); (figure != nullptr))
		{
			if (m_pField->hasMovingFigures() ||
				!figure->isClicked() ||
				(figure->isWhite() != m_bPlayerWhite))
				continue;

			if (m_pField->haveSelectedFigure())
				m_pField->getSelectedFigure()->select(false);

			figure->select();

			m_pField->setSelectedData(figure, cell.get());

			// Unselect old neighbours
			for (const auto& neighbour : m_pField->getAvailablePaths())
				neighbour->select(false);

			m_pField->computeAvailablePaths(i);

			// Unselect new neighbours
			for (const auto& neighbour : m_pField->getAvailablePaths())
				neighbour->select();
		}
	}
}

void Game::processPlayerMove()
{
	if (!m_pField->haveSelectedFigure())
		return;

	for (const auto& cell : m_pField->getAvailablePaths())
	{
		if (cell->isClicked())
		{
			const auto& selected_data = m_pField->getSelectedData();

			selected_data.first->setMoveDoneCallback([=]()
			{
				selected_data.second->getFigureInside()->select(false);

				selected_data.second->setFigureInside(nullptr);

				cell->setFigureInside(selected_data.first);

				m_pField->setMovingFigures(false);

				this->nextTurn();
			});

			m_pField->moveSelectedFigureToCell(cell);

			this->setState(Game::State::PLAYER_MOVE);
		}
	}
}

void Game::processAI()
{
	this->messageText(L"Ход оппонента...");

	if (m_State != Game::State::AI_MOVE)
		m_pAIPlayer->think();
}

void Game::nextTurn() noexcept
{
	if (m_pField->getSelectedFigure() != nullptr)
		m_pField->getSelectedFigure()->select(false);

	m_pField->setSelectedData(nullptr, nullptr);

	switch (m_State)
	{
		case State::PLAYER_MOVE:
			this->setState(State::AI_THINK);
		break;

		case State::AI_MOVE:
			this->setState(State::PLAYER_THINK);
		break;

		default:
		break;
	}
}

const bool Game::isPlayerTurn() const noexcept
{
	return (m_State == State::PLAYER_THINK || m_State == State::PLAYER_MOVE);
}

void Game::messageText(const std::wstring& msg)
{
	m_pText->setString(msg);
}
