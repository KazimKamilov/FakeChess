#ifndef _FAKE_CHESS_RENDERER_HPP_
#define _FAKE_CHESS_RENDERER_HPP_

#include <memory>
#include <string>

#include "SFML/System/Vector2.hpp"

namespace sf
{
class RenderWindow;
class Font;
class Text;
}

class Field;
class AIPlayer;


class Game final
{
public:
	enum class State : uint8_t
	{
		NOT_STARTED,
		PLAYER_MOVE,
		PLAYER_THINK,
		AI_MOVE,
		AI_THINK,
		HAVE_WINNER
	};

private:
	sf::RenderWindow* m_pWindow;
	std::unique_ptr<sf::Font> m_pFont;
	std::unique_ptr<sf::Text> m_pText;

	sf::Vector2i m_Size {800, 600};

	std::unique_ptr<Field> m_pField;
	std::unique_ptr<AIPlayer> m_pAIPlayer;

	// Change this to choose a team
	static constexpr bool m_bPlayerWhite = false;

	State m_State = State::NOT_STARTED;

public:
	Game(sf::RenderWindow* window);
	~Game();

	void update(const float delta);

	void setViewSize(const sf::Vector2i& size) noexcept { m_Size = size; }

	void nextTurn() noexcept;

	void setState(const State state) noexcept { m_State = state; }

	// True is player moves, false - if AI
	const bool isPlayerTurn() const noexcept;

	const auto& getField() const noexcept { return m_pField; }
	const State getState() const noexcept { return m_State; }

private:
	void waitForStart();
	void checkAndEndGameIfWin();

	void processPlayer();
	void processPlayerSelect();
	void processPlayerMove();

	void processAI();

	void messageText(const std::wstring& msg);

};

#endif
