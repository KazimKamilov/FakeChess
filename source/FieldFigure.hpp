#ifndef _FAKE_CHESS_FIELD_FIGURE_HPP_
#define _FAKE_CHESS_FIELD_FIGURE_HPP_

#include <utility>
#include <functional>
#include <inttypes.h>

#include "FieldElement.hpp"

#include "SFML/System/Vector2.hpp"


class FieldFigure final : public FieldElement
{
public:
	enum class MoveDirection : uint8_t
	{
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

private:
	bool m_bWhite = false;

	MoveDirection m_MoveDirection = MoveDirection::NONE;
	std::pair<sf::Vector2f, sf::Vector2f> m_MovePath;
	std::function<void()> m_MoveDoneCallback;

public:
	FieldFigure(Field* field, const float diameter, const bool white);
	virtual ~FieldFigure();

	virtual void update(const float delta) override;

	virtual const bool isCoordsInside(const sf::Vector2i& coords) const override;

	virtual void setPosition(const sf::Vector2f& position) noexcept override;

	void select(const bool selected = true);

	void setMovePath(const sf::Vector2f& from, const sf::Vector2f& to) { m_MovePath = {from, to}; }

	void startMove(const MoveDirection direction) { m_MoveDirection = direction; }

	const bool isMoving() const { return (m_MoveDirection != MoveDirection::NONE); }
	const bool isWhite() const noexcept { return m_bWhite; }

	template<typename F>
	void setMoveDoneCallback(F&& callback)
	{
		m_MoveDoneCallback = std::move(callback);
	}

};

#endif
