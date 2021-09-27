#include <bitset>
#include <iostream>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "catch.hpp"
#include "ecs.hpp"

TEST_CASE("item Config Setting Values", "[item]") {
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

TEST_CASE("System<> Allocation", "[system]") {
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

TEST_CASE("System<T, ...R> Allocation", "[system]") {
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
	ComponentConfig<char, std::string, int, float, std::vector<std::size_t>> cc2;

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

TEST_CASE("System<> is_alive()", "[system]") {
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

TEST_CASE("System<T, ...R> is_alive()", "[system]") {
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

TEST_CASE("System<> get", "[system]") {
	System sys;
	ComponentConfig cc;
	Entity e = sys.make(cc);
	REQUIRE(sys.get(e).has_value());
	sys.erase(e);
	REQUIRE_FALSE(sys.get(e).has_value());
}

TEST_CASE("System<T, R...> get", "[system]") {
	System<int, std::string, char> sys;
	ComponentConfig<int, std::string, char> cc;
	Entity e1 = sys.make(cc);
	REQUIRE(sys.get(e1).has_value());
	REQUIRE_FALSE(sys.get(e1)->get<0>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<1>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<2>().has_value());

	cc.get<int>() = 13;
	cc.get<std::string>() = "beer & pizza";
	Entity e2 = sys.make(cc);
	REQUIRE(sys.get(e1).has_value());
	REQUIRE_FALSE(sys.get(e1)->get<0>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<1>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<2>().has_value());
	REQUIRE(sys.get(e2).has_value());
	REQUIRE(*sys.get(e2)->get<0>() == 13);
	REQUIRE(*sys.get(e2)->get<1>() == "beer & pizza");
	REQUIRE_FALSE(sys.get(e2)->get<2>().has_value());
	
	*sys.get(e2)->get<0>() = 5;
	REQUIRE(*sys.get(e2)->get<0>() == 5);
	REQUIRE(*sys.get(e2)->get<1>() == "beer & pizza");
	REQUIRE_FALSE(sys.get(e2)->get<2>().has_value());

	*sys.get(e2)->get<1>() = "chips & salsa";
	REQUIRE(*sys.get(e2)->get<0>() == 5);
	REQUIRE(*sys.get(e2)->get<1>() == "chips & salsa");
	REQUIRE_FALSE(sys.get(e2)->get<2>().has_value());

	cc.get<1>() = boost::none;
	cc.get<2>() = 'f';
	sys.erase(e1);
	Entity e3 = sys.make(cc);
	REQUIRE_FALSE(sys.get(e1).has_value());
	REQUIRE(*sys.get(e2)->get<0>() == 5);
	REQUIRE(*sys.get(e2)->get<1>() == "chips & salsa");
	REQUIRE(*sys.get(e3)->get<0>() == 13);
	REQUIRE_FALSE(sys.get(e3)->get<1>().has_value());
	REQUIRE(*sys.get(e3)->get<2>() == 'f');
}

