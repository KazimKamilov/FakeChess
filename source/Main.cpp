#include "Application.hpp"
#include "Log.hpp"


class LogBuddy final
{
	const std::string m_filename {"game.log"};

public:
	LogBuddy()
	{
		auto& log_var = log::get();

		log_var.open(m_filename);

		if (!log_var.good())
			throw std::runtime_error("Cannot open Fake Chess log file!");
	}

	~LogBuddy()
	{
		auto& log_var = log::get();
		log_var << "Fake Chess is now closed..." << endl;

		log_var.close();
	}

	void say_hello()
	{
		log::get(log::type::SEPARATOR);
		log::get() << "Fake Chess log \"" << m_filename << "\" created!" << endl;
		log::get(log::type::SEPARATOR);
	}
};

int main(int argc, char* argv[])
{
	LogBuddy log_buddy;
	log_buddy.say_hello();

	auto app {std::make_unique<Application>()};

	app->init();

	app->update();

	app->destroy();

	return 0;
}
