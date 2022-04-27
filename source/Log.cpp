#include "Log.hpp"

std::ofstream& log::get(const type msg_type) noexcept
{
	static log self;

	switch (msg_type)
	{
		case type::INFO:
			self.m_file << "INFO: ";
		break;

		case type::WARNING:
			self.m_file << "WARNING: ";
		break;

		case type::ERROR:
			self.m_file << "ERROR: ";
		break;

		case type::FATAL:
			self.m_file << "FATAL ERROR: ";
		break;

		case type::SEPARATOR:
			self.m_file << std::string(64u, '=') << endl;
		break;

		default:
		break;
	}

	return self.m_file;
}
