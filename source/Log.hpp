#ifndef _FAKE_CHESS_LOG_HPP_
#define _FAKE_CHESS_LOG_HPP_

#include <fstream>
#include <string>

using std::endl;

class log final
{
	friend class LogBuddy;

	std::ofstream m_file;

	log() noexcept = default;
	~log() noexcept = default;
	log(log const&) = delete;
	log(log const&&) = delete;
	log& operator=(log const&) = delete;
	log& operator=(log const&&) = delete;

public:
	enum class type : uint8_t
	{
		MESSAGE = 0x00u,
		INFO,
		WARNING,
		ERROR,
		FATAL,
		SEPARATOR
	};

	static std::ofstream& get(const type msg_type = type::MESSAGE) noexcept;

};

#endif
