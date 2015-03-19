#include <elliptics/session.hpp>

#include <cocaine/framework/dispatch.hpp>
#include <cocaine/framework/logging.hpp>

#include <iostream>

using namespace ioremap;

static void noop_function(cocaine::framework::response_ptr)
{
}

class app_t {
public:
	std::string id;
	std::shared_ptr<cocaine::framework::logger_t> cocaine_log;

	std::unique_ptr<elliptics::file_logger> elliptics_logger;
	std::unique_ptr<elliptics::node> node;
	std::unique_ptr<elliptics::session> session;

	app_t(cocaine::framework::dispatch_t& dispatch)
		: id(dispatch.id())
		, cocaine_log(dispatch.service_manager()->get_system_logger())
	{
		COCAINE_LOG_INFO(cocaine_log, "%s, registering event handler(s)", id.c_str());

		dispatch.on("test_event", this, &app_t::test_event);
		dispatch.on("init", this, &app_t::init);

		COCAINE_LOG_INFO(cocaine_log, "%s, application started", id.c_str());
	}

	void init(const std::string &, const std::vector<std::string> &chunks, cocaine::framework::response_ptr response) {
		elliptics::exec_context context = elliptics::exec_context::from_raw(chunks[0].c_str(), chunks[0].size());

		const std::string log_path("/tmp/test_app_cpp");

		elliptics_logger.reset(new elliptics::file_logger(log_path.c_str(), DNET_LOG_DEBUG));
		node.reset(new elliptics::node(elliptics::logger(*elliptics_logger, blackhole::log::attributes_t())));

		node->add_remote("localhost:1025:2");

		session.reset(new elliptics::session(*node));
		session->set_groups({1});

		elliptics::async_reply_result result = session->reply(context, std::string("initialized"), elliptics::exec_context::final);
		result.connect(std::bind(noop_function, response));
	}

	void test_event(const std::string &cocaine_event, const std::vector<std::string> &chunks, cocaine::framework::response_ptr response) {
		if (!session) {
			response->error(-EINVAL, "I'm not initialized yet");
			return;
		}

		elliptics::exec_context context = elliptics::exec_context::from_raw(chunks[0].c_str(), chunks[0].size());

		COCAINE_LOG_INFO(cocaine_log, "echo: event: %s, data-size: %ld", cocaine_event.c_str(), context.data().size());

		elliptics::async_reply_result result = session->reply(context, context.data(), elliptics::exec_context::final);
		result.connect(std::bind(noop_function, response));
	}
};

int main(int argc, char **argv) {
	return cocaine::framework::run<app_t>(argc, argv);
}
