#include <bitset>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <iostream>
#include <limits>

#include "catch.hpp"
#include "stupid_ecs.hpp"

using namespace secs;

template <uint N>
struct loop {
	void operator()() {
		REQUIRE(std::numeric_limits<detail::bitmask<N>>::digits > N);
		loop<N-1>()();
	}
};
template <>
struct loop<0> {
	void operator()() {
		REQUIRE(std::numeric_limits<detail::bitmask<0>>::digits > 0);
	}
};
TEST_CASE("autosize bitmask", "[detail]") {
	loop<63>()();
}

TEST_CASE("item Config Setting Values", "[item]")
{
	ComponentConfig<int, std::string, char> cc;
	REQUIRE(cc.get<int>() == cc.item);
	REQUIRE(cc.get<std::string>() == cc.tail.item);
	REQUIRE(cc.get<char>() == cc.tail.tail.item);

	cc.item = 7;
	REQUIRE(cc.item == 7);
	REQUIRE(cc.get<int>() == 7);

	REQUIRE(cc.get<int>() == cc.item);
	REQUIRE(cc.get<std::string>() == cc.tail.item);
	REQUIRE(cc.get<char>() == cc.tail.tail.item);

	cc.item = boost::none;
	REQUIRE_FALSE(cc.item.has_value());
	REQUIRE_FALSE(cc.get<int>().has_value());

	REQUIRE(cc.get<int>() == cc.item);
	REQUIRE(cc.get<std::string>() == cc.tail.item);
	REQUIRE(cc.get<char>() == cc.tail.tail.item);

	cc.get<int>() = 13;
	REQUIRE(cc.item == 13);
	REQUIRE(cc.get<int>() == 13);

	REQUIRE(cc.get<int>() == cc.item);
	REQUIRE(cc.get<std::string>() == cc.tail.item);
	REQUIRE(cc.get<char>() == cc.tail.tail.item);
}

TEST_CASE("System<> Allocation", "[system]")
{
	System system;
	ComponentConfig cc;

	Entity e1 = system.make(cc);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	Entity e2 = system.make(cc);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);

	Entity e3 = system.make(cc);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	system.erase(e2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	Entity e4 = system.make(cc);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);
	REQUIRE(e4.generation == 1);
	REQUIRE(e4.index == 1);
};

TEST_CASE("System<T, ...R> Allocation", "[system]")
{
	System<int> sys1;
	ComponentConfig<int> cc1;

	Entity e1 = sys1.make(cc1);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	Entity e2 = sys1.make(cc1);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);

	Entity e3 = sys1.make(cc1);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	sys1.erase(e2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	Entity e4 = sys1.make(cc1);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);
	REQUIRE(e4.generation == 1);
	REQUIRE(e4.index == 1);

	System<char, std::string, int, float, std::vector<std::size_t>> sys2;
	ComponentConfig<char, std::string, int, float, std::vector<std::size_t>>
			cc2;

	e1 = sys2.make(cc2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	e2 = sys2.make(cc2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);

	e3 = sys2.make(cc2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	sys2.erase(e2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	e4 = sys2.make(cc2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);
	REQUIRE(e4.generation == 1);
	REQUIRE(e4.index == 1);
};

TEST_CASE("System<> is_alive()", "[system]")
{
	System system;
	ComponentConfig cc;

	Entity e1 = system.make(cc);
	REQUIRE(system.is_alive(e1));

	Entity e2 = system.make(cc);
	REQUIRE(system.is_alive(e1));
	REQUIRE(system.is_alive(e2));

	system.erase(e1);
	REQUIRE_FALSE(system.is_alive(e1));
	REQUIRE(system.is_alive(e2));

	Entity e3 = system.make(cc);
	REQUIRE_FALSE(system.is_alive(e1));
	REQUIRE(system.is_alive(e2));
	REQUIRE(system.is_alive(e3));
}

TEST_CASE("System<T, ...R> is_alive()", "[system]")
{
	System<int> sys1;
	ComponentConfig<int> cc1;

	Entity e1 = sys1.make(cc1);
	REQUIRE(sys1.is_alive(e1));

	Entity e2 = sys1.make(cc1);
	REQUIRE(sys1.is_alive(e1));
	REQUIRE(sys1.is_alive(e2));

	sys1.erase(e1);
	REQUIRE_FALSE(sys1.is_alive(e1));
	REQUIRE(sys1.is_alive(e2));

	Entity e3 = sys1.make(cc1);
	REQUIRE_FALSE(sys1.is_alive(e1));
	REQUIRE(sys1.is_alive(e2));
	REQUIRE(sys1.is_alive(e3));

	System<int, char, std::string, double> sys2;
	ComponentConfig<int, char, std::string, double> cc2;

	e1 = sys2.make(cc2);
	REQUIRE(sys2.is_alive(e1));

	e2 = sys2.make(cc2);
	REQUIRE(sys2.is_alive(e1));
	REQUIRE(sys2.is_alive(e2));

	sys2.erase(e1);
	REQUIRE_FALSE(sys2.is_alive(e1));
	REQUIRE(sys2.is_alive(e2));

	e3 = sys2.make(cc2);
	REQUIRE_FALSE(sys2.is_alive(e1));
	REQUIRE(sys2.is_alive(e2));
	REQUIRE(sys2.is_alive(e3));
}

TEST_CASE("System<> get", "[system]")
{
	System sys;
	ComponentConfig cc;
	Entity e = sys.make(cc);
	REQUIRE(sys[e].has_value());
	sys.erase(e);
	REQUIRE_FALSE(sys[e].has_value());
}

TEST_CASE("System<T, R...> get", "[system]")
{
	System<int, std::string, char> sys;
	ComponentConfig<int, std::string, char> cc;
	Entity e1 = sys.make(cc);
	REQUIRE(sys[e1].has_value());
	REQUIRE_FALSE(sys[e1]->get<0>().has_value());
	REQUIRE_FALSE(sys[e1]->get<1>().has_value());
	REQUIRE_FALSE(sys[e1]->get<2>().has_value());

	cc.get<int>()         = 13;
	cc.get<std::string>() = "beer & pizza";
	Entity e2             = sys.make(cc);
	REQUIRE(sys[e1].has_value());
	REQUIRE_FALSE(sys[e1]->get<0>().has_value());
	REQUIRE_FALSE(sys[e1]->get<1>().has_value());
	REQUIRE_FALSE(sys[e1]->get<2>().has_value());
	REQUIRE(sys[e2].has_value());
	REQUIRE(*sys[e2]->get<0>() == 13);
	REQUIRE(*sys[e2]->get<1>() == "beer & pizza");
	REQUIRE_FALSE(sys[e2]->get<2>().has_value());

	*sys[e2]->get<0>() = 5;
	REQUIRE(*sys[e2]->get<0>() == 5);
	REQUIRE(*sys[e2]->get<1>() == "beer & pizza");
	REQUIRE_FALSE(sys[e2]->get<2>().has_value());

	*sys[e2]->get<1>() = "chips & salsa";
	REQUIRE(*sys[e2]->get<0>() == 5);
	REQUIRE(*sys[e2]->get<1>() == "chips & salsa");
	REQUIRE_FALSE(sys[e2]->get<2>().has_value());

	cc.get<1>() = boost::none;
	cc.get<2>() = 'f';
	sys.erase(e1);
	Entity e3 = sys.make(cc);
	REQUIRE_FALSE(sys[e1].has_value());
	REQUIRE(*sys[e2]->get<0>() == 5);
	REQUIRE(*sys[e2]->get<1>() == "chips & salsa");
	REQUIRE(*sys[e3]->get<0>() == 13);
	REQUIRE_FALSE(sys[e3]->get<1>().has_value());
	REQUIRE(*sys[e3]->get<2>() == 'f');
}

TEST_CASE("System Iterator<>", "[system]")
{
	System<int, char, std::string> sys;
	std::vector<Entity> ent;
	ComponentConfig<int, char, std::string> cc;

	for (int i = 0; i < 25; i++) {
		cc.get<int>()  = i;
		cc.get<char>() = i % 3 == 0 ? boost::none : boost::make_optional('F');
		cc.get<std::string>() =
				i % 5 == 0 ? boost::make_optional("Hello World") : boost::none;

		ent.push_back(sys.make(cc));
	}

	int index = 0;
	for (auto i : sys.iter<>()) {
		REQUIRE(i.get<int &>().has_value());
		REQUIRE(*i.get<int &>() == index);
		REQUIRE(*sys.get<int>(ent[index]) == index);

		(*i.get<int &>())++;
		REQUIRE(*i.get<int &>() == index + 1);
		REQUIRE(*sys.get<int>(ent[index]) == index + 1);

		if (i.get<char &>())
			REQUIRE(*i.get<char &>() == 'F');

		if (i.get<std::string &>()) {
			REQUIRE(*i.get<std::string &>() == "Hello World");
			*i.get<std::string &>() = "Beer & Pizza";
			REQUIRE(*i.get<std::string &>() == "Beer & Pizza");
		}

		index++;
	}
	for (auto i = 0; i < (int)ent.size(); ++i) {
		REQUIRE(sys.get<int>(ent[i]).has_value());
		REQUIRE(*sys.get<int>(ent[i]) == i + 1);
		if (sys.get<char>(ent[i]))
			REQUIRE(*sys.get<char>(ent[i]) == 'F');
		if (sys.get<std::string>(ent[i]))
			REQUIRE(*sys.get<std::string>(ent[i]) == "Beer & Pizza");
	}

	*sys.get<int>(ent[20])         = 99;
	*sys.get<char>(ent[20])        = 'a';
	*sys.get<std::string>(ent[20]) = "sentinel";
	REQUIRE(*sys.get<int>(ent[20]) == 99);
	REQUIRE(*sys.get<char>(ent[20]) == 'a');
	REQUIRE(*sys.get<std::string>(ent[20]) == "sentinel");
	sys.erase(ent[20]);

	for (auto i : sys.iter()) {
		if (i.get<int &>())
			REQUIRE_FALSE(*i.get<int &>() == 99);
		if (i.get<char &>())
			REQUIRE_FALSE(*i.get<char &>() == 'a');
		if (i.get<std::string &>())
			REQUIRE_FALSE(*i.get<std::string &>() == "sentinel");
	}
}

TEST_CASE("System Iterator<T...>", "[system]")
{
	System<int, char, std::string> sys;
	std::vector<Entity> ent;
	ComponentConfig<int, char, std::string> cc;

	for (int i = 0; i < 25; i++) {
		cc.get<int>()  = i;
		cc.get<char>() = i % 3 == 0 ? boost::none : boost::make_optional('F');
		cc.get<std::string>() =
				i % 5 == 0 ? boost::make_optional("Hello World") : boost::none;

		ent.push_back(sys.make(cc));
	}

	int index = 0;
	for (auto [num] : sys.iter<int>()) {
		REQUIRE(num == index);
		REQUIRE(*sys.get<int>(ent[index]) == index);

		num++;
		REQUIRE(num == index + 1);
		REQUIRE(*sys.get<int>(ent[index]) == index + 1);

		index++;
	}
	for (auto i = 0; i < (int)ent.size(); ++i) {
		REQUIRE(*sys.get<int>(ent[i]) == i + 1);
	}

	int count = 0;
	for (auto [n, c, s] : sys.iter<int, char, std::string>()) {
		REQUIRE((n - 1) % 5 == 0);
		REQUIRE(c == 'F');
		c = 'j';
		REQUIRE(c == 'j');
		REQUIRE(s == "Hello World");
		count++;
	}
	REQUIRE(count == 3);

	count = 0;
	for (auto [c, s, n] : sys.iter<char, std::string, int>()) {
		REQUIRE((n - 1) % 5 == 0);
		REQUIRE(c == 'j');
		REQUIRE(s == "Hello World");
		count++;
	}
	REQUIRE(count == 3);

	for (auto i = 0; i < (int)ent.size(); ++i) {
		auto e = ent[i];
		REQUIRE(*sys.get<int>(e) == i + 1);
		if (sys.get<std::string>(e))
			REQUIRE(*sys.get<std::string>(e) == "Hello World");
		if (sys.get<std::string>(e) && sys.get<char>(e))
			REQUIRE(*sys.get<char>(ent[i]) == 'j');
		if (!sys.get<std::string>(e) && sys.get<char>(e))
			REQUIRE(*sys.get<char>(ent[i]) == 'F');
	}

	*sys.get<int>(ent[10])         = 99;
	*sys.get<char>(ent[10])        = 'a';
	*sys.get<std::string>(ent[10]) = "sentinel";
	REQUIRE(*sys.get<int>(ent[10]) == 99);
	REQUIRE(*sys.get<char>(ent[10]) == 'a');
	REQUIRE(*sys.get<std::string>(ent[10]) == "sentinel");
	sys.erase(ent[10]);

	index = 0;
	for (auto [n] : sys.iter<int>()) REQUIRE_FALSE(n == 99);
	for (auto [c] : sys.iter<char>()) REQUIRE_FALSE(c == 'a');
	for (auto [s] : sys.iter<std::string>()) REQUIRE_FALSE(s == "sentinel");
}
