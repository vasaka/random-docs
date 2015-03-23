#include <elliptics/cppdef.h>

#include <iostream>

using namespace ioremap;

struct Ctx {
	Ctx()
	: iflags(0)
	{}

	std::vector<int> groups;
	uint64_t iflags;
	dnet_iterator_range key_range;
	dnet_time time_begin, time_end;
	std::unique_ptr<ioremap::elliptics::session> session;
	std::vector<dnet_route_entry> routes;
};

void send_message(Ctx &ctx, const dnet_addr &node) {
	std::cout << "Iterating node: " << dnet_addr_string(&node) << ":" << node.family << std::endl;
	std::vector<dnet_iterator_range> ranges;
	if (ctx.iflags & DNET_IFLAGS_KEY_RANGE)
		ranges.push_back(ctx.key_range);

	dnet_id id;
	memset(&id, 0, sizeof(id));
	ctx.session->set_groups({1});
	ctx.session->transform("TEST_KEY", id);

	std::string event("test_app@test_event");
	std::string data("some random data!\n");
	auto result = ctx.session->exec(&id, -1, event, data);
	for (auto it = result.begin(); it != result.end(); ++it) {
		if (it->error()) {
			elliptics::error_info error = it->error();
			std::cerr << dnet_addr_string(it->address())
			<< ": failed to process: \"" << error.message() << "\": " << error.code() << std::endl;
		} else {
			elliptics::exec_context context = it->context();
			if (context.is_null()) {
				std::cout << dnet_addr_string(it->address())
				<< ": acknowledge" << std::endl;
			} else {
				std::cout << dnet_addr_string(context.address())
				<< ": " << context.event()
				<< " \"" << context.data().to_string() << "\"" << std::endl;
			}
		}
	}
}

dnet_addr parse_addr(const std::string& addr) {
	dnet_addr ret;
	int port, family;
	memset(&ret, 0, sizeof(ret));
	ret.addr_len = sizeof(ret.addr);

	std::vector<char> tmp_addr(addr.begin(), addr.end());
	tmp_addr.push_back('\0');

	int err = dnet_parse_addr(tmp_addr.data(), &port, &family);
	if (err) {
		std::cerr << "Wrong remote addr: " << addr << "\n" << std::endl;
		exit(1);
	}
	ret.family = family;
	dnet_fill_addr(&ret, tmp_addr.data(), port, SOCK_STREAM, IPPROTO_TCP);
	return ret;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "provide node address, please" << std::endl;
		return 0;
	}

	dnet_log_level log_level = ioremap::elliptics::file_logger::parse_level("info");
	ioremap::elliptics::file_logger logger("/tmp/send_msg_log", log_level);
	ioremap::elliptics::node node(ioremap::elliptics::logger(logger, blackhole::log::attributes_t()));
	node.add_remote(argv[1]);

	Ctx ctx;
	ctx.session.reset(new ioremap::elliptics::session(node));
	ctx.routes = ctx.session->get_routes();
	send_message(ctx, parse_addr(argv[1]));

	return 0;
}
