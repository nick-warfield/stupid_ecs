#include <bitset>
#include <iostream>
#include "catch.hpp"
#include "ecs.hpp"

TEST_CASE("Component Config Bitmask", "[component]") {
	ComponentConfig<int, std::string, char> cc;
	REQUIRE_FALSE(cc.get<COMPONENT_01_FLAG>().has_value());
	REQUIRE_FALSE(cc.get<COMPONENT_02_FLAG>().has_value());
	REQUIRE_FALSE(cc.get<COMPONENT_03_FLAG>().has_value());
	REQUIRE(cc.bitmask() == 0);

	cc.component = 9;
	REQUIRE(cc.bitmask() == (COMPONENT_01_FLAG));
	cc.remainder.remainder.component = 'y';
	REQUIRE(cc.bitmask() == (COMPONENT_03_FLAG | COMPONENT_01_FLAG));
	cc.remainder.component = "word";
	REQUIRE(cc.bitmask() == (COMPONENT_03_FLAG | COMPONENT_02_FLAG | COMPONENT_01_FLAG));
	cc.component = std::nullopt;
	REQUIRE(cc.bitmask() == (COMPONENT_03_FLAG | COMPONENT_02_FLAG));
	cc.component = 7;
	REQUIRE(cc.bitmask() == (COMPONENT_03_FLAG | COMPONENT_02_FLAG | COMPONENT_01_FLAG));
	cc.remainder.component = std::nullopt;
	REQUIRE(cc.bitmask() == (COMPONENT_03_FLAG | COMPONENT_01_FLAG));
	cc.remainder.remainder.component = std::nullopt;
	REQUIRE(cc.bitmask() == (COMPONENT_01_FLAG));
	cc.component = std::nullopt;
	REQUIRE(cc.bitmask() == 0);
}

TEST_CASE("Component Config Setting Values", "[component]") {
	ComponentConfig<int, std::string, char> cc;
	REQUIRE(cc.get<COMPONENT_01_FLAG>() == cc.component);
	REQUIRE(cc.get<COMPONENT_02_FLAG>() == cc.remainder.component);
	REQUIRE(cc.get<COMPONENT_03_FLAG>() == cc.remainder.remainder.component);

	cc.component = 7;
	REQUIRE(cc.component == 7);
	REQUIRE(cc.get<COMPONENT_01_FLAG>() == 7);

	REQUIRE(cc.get<COMPONENT_01_FLAG>() == cc.component);
	REQUIRE(cc.get<COMPONENT_02_FLAG>() == cc.remainder.component);
	REQUIRE(cc.get<COMPONENT_03_FLAG>() == cc.remainder.remainder.component);

	cc.component = std::nullopt;
	REQUIRE_FALSE(cc.component.has_value());
	REQUIRE_FALSE(cc.get<COMPONENT_01_FLAG>().has_value());

	REQUIRE(cc.get<COMPONENT_01_FLAG>() == cc.component);
	REQUIRE(cc.get<COMPONENT_02_FLAG>() == cc.remainder.component);
	REQUIRE(cc.get<COMPONENT_03_FLAG>() == cc.remainder.remainder.component);

	cc.get<COMPONENT_01_FLAG>() = 13;
	REQUIRE(cc.component == 13);
	REQUIRE(cc.get<COMPONENT_01_FLAG>() == 13);

	REQUIRE(cc.get<COMPONENT_01_FLAG>() == cc.component);
	REQUIRE(cc.get<COMPONENT_02_FLAG>() == cc.remainder.component);
	REQUIRE(cc.get<COMPONENT_03_FLAG>() == cc.remainder.remainder.component);
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

TEST_CASE("System<> has_component()", "[system]") {
	System system;
	ComponentConfig cc;

	Entity e1 = system.make(cc);
	REQUIRE(system.has_component(e1, COMPONENT_ALIVE));
	for (int i = 1; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(system.has_component(e1, i));
	}

	Entity e2 = system.make(cc);
	REQUIRE(system.has_component(e1, COMPONENT_ALIVE));
	REQUIRE(system.has_component(e2, COMPONENT_ALIVE));
	for (int i = 1; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(system.has_component(e1, i));
		REQUIRE_FALSE(system.has_component(e2, i));
	}

	system.erase(e1);
	REQUIRE_FALSE(system.has_component(e1, COMPONENT_ALIVE));
	REQUIRE(system.has_component(e2, COMPONENT_ALIVE));
	for (int i = 1; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(system.has_component(e1, i));
		REQUIRE_FALSE(system.has_component(e2, i));
	}

	Entity e3 = system.make(cc);
	REQUIRE_FALSE(system.has_component(e1, COMPONENT_ALIVE));
	REQUIRE(system.has_component(e2, COMPONENT_ALIVE));
	REQUIRE(system.has_component(e3, COMPONENT_ALIVE));
	for (int i = 1; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(system.has_component(e1, i));
		REQUIRE_FALSE(system.has_component(e2, i));
		REQUIRE_FALSE(system.has_component(e3, i));
	}
}

TEST_CASE("System<T, ...R> has_component()", "[system]") {
	System<int> sys1;
	ComponentConfig<int> cc1;

	Entity e1 = sys1.make(cc1);
	REQUIRE(sys1.has_component(e1, COMPONENT_ALIVE));
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_01_FLAG));
	for (int i = COMPONENT_02_FLAG; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(sys1.has_component(e1, i));
	}

	cc1.get<1>() = 7;
	Entity e2 = sys1.make(cc1);
	REQUIRE(sys1.has_component(e1, COMPONENT_ALIVE));
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_01_FLAG));
	REQUIRE(sys1.has_component(e2, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e2, COMPONENT_01_FLAG));
	for (int i = COMPONENT_02_FLAG; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(sys1.has_component(e1, i));
		REQUIRE_FALSE(sys1.has_component(e2, i));
	}

	sys1.erase(e1);
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_ALIVE));
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_01_FLAG));
	REQUIRE(sys1.has_component(e2, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e2, COMPONENT_01_FLAG));
	for (int i = COMPONENT_02_FLAG; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(sys1.has_component(e1, i));
		REQUIRE_FALSE(sys1.has_component(e2, i));
	}

	Entity e3 = sys1.make(cc1);
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_ALIVE));
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_01_FLAG));
	REQUIRE(sys1.has_component(e2, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e2, COMPONENT_01_FLAG));
	REQUIRE(sys1.has_component(e3, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e3, COMPONENT_01_FLAG));
	for (int i = COMPONENT_02_FLAG; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(sys1.has_component(e1, i));
		REQUIRE_FALSE(sys1.has_component(e2, i));
		REQUIRE_FALSE(sys1.has_component(e3, i));
	}

	sys1.erase(e2);
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_ALIVE));
	REQUIRE_FALSE(sys1.has_component(e1, COMPONENT_01_FLAG));
	REQUIRE_FALSE(sys1.has_component(e2, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e2, COMPONENT_01_FLAG));
	REQUIRE(sys1.has_component(e3, COMPONENT_ALIVE));
	REQUIRE(sys1.has_component(e3, COMPONENT_01_FLAG));
	for (int i = COMPONENT_02_FLAG; i != COMPONENT_ALIVE; i <<= 1) {
		REQUIRE_FALSE(sys1.has_component(e1, i));
		REQUIRE_FALSE(sys1.has_component(e2, i));
		REQUIRE_FALSE(sys1.has_component(e3, i));
	}
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
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_01_FLAG>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_02_FLAG>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_03_FLAG>().has_value());

	cc.get<COMPONENT_01_FLAG>() = 13;
	cc.get<COMPONENT_02_FLAG>() = "beer & pizza";
	Entity e2 = sys.make(cc);
	REQUIRE(sys.get(e1).has_value());
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_01_FLAG>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_02_FLAG>().has_value());
	REQUIRE_FALSE(sys.get(e1)->get<COMPONENT_03_FLAG>().has_value());
	REQUIRE(sys.get(e2).has_value());
	REQUIRE(sys.get(e2)->get<COMPONENT_01_FLAG>() == 13);
	REQUIRE(sys.get(e2)->get<COMPONENT_02_FLAG>()->get() == "beer & pizza");
	REQUIRE_FALSE(sys.get(e2)->get<COMPONENT_03_FLAG>().has_value());
	
	sys.get(e2)->get<COMPONENT_01_FLAG>()->get() = 5;
	REQUIRE(sys.get(e2)->get<COMPONENT_01_FLAG>() == 5);
	REQUIRE(sys.get(e2)->get<COMPONENT_02_FLAG>()->get() == "beer & pizza");
	REQUIRE_FALSE(sys.get(e2)->get<COMPONENT_03_FLAG>().has_value());

	sys.get(e2)->get<COMPONENT_02_FLAG>()->get() = "chips & salsa";
	REQUIRE(sys.get(e2)->get<COMPONENT_01_FLAG>() == 5);
	REQUIRE(sys.get(e2)->get<COMPONENT_02_FLAG>()->get() == "chips & salsa");
	REQUIRE_FALSE(sys.get(e2)->get<COMPONENT_03_FLAG>().has_value());

	cc.get<COMPONENT_02_FLAG>() = std::nullopt;
	cc.get<COMPONENT_03_FLAG>() = 'f';
	sys.erase(e1);
	Entity e3 = sys.make(cc);
	REQUIRE_FALSE(sys.get(e1).has_value());
	REQUIRE(sys.get(e2)->get<COMPONENT_01_FLAG>() == 5);
	REQUIRE(sys.get(e2)->get<COMPONENT_02_FLAG>()->get() == "chips & salsa");
	REQUIRE(sys.get(e3)->get<COMPONENT_01_FLAG>() == 13);
	REQUIRE_FALSE(sys.get(e3)->get<COMPONENT_02_FLAG>().has_value());
	REQUIRE(sys.get(e3)->get<COMPONENT_03_FLAG>() == 'f');
}

// TEST_CASE("Entity dereferencing") {
// 	System system;
// 	ComponentConfig<int, std::string> cc;
// 	cc.get<COMPONENT_01_FLAG>() = 7;
// 	cc.get<COMPONENT_02_FLAG>() = "word";
// 
// 	Entity e = system.make(cc);
// 	REQUIRE((*e).has_value());
// 	REQUIRE((*e)->data1 == 7);
// 	REQUIRE((*e)->data2->get() == "word");
// 	REQUIRE(e->data1 == 7);
// 	REQUIRE(e->data2->get() == "word");
// 
// 	system.erase(e);
// 	REQUIRE_FALSE((*e).has_value());
// 	REQUIRE_THROWS((*e).value().data1 == 7);
// 	REQUIRE_THROWS((*e).value().data2->get() == "word");
// }
// 
// TEST_CASE("System Read and Write") {
// 	System system;
// 	ComponentConfig<int, std::string> cc;
// 	cc.get<COMPONENT_01_FLAG>() = 5;
// 	cc.get<COMPONENT_02_FLAG>() = "pizza";
// 	Entity e = system.make(cc);
// 
// 	REQUIRE(e->data1.has_value());
// 	REQUIRE(e->data1 == 5);
// 	REQUIRE(e->data2.has_value());
// 	REQUIRE(e->data2->get() == "pizza");
// 
// 	*e->data1 += 2;
// 	REQUIRE(e->data1 == 7);
// 	e->data2->get() = "beer";
// 	REQUIRE(e->data2->get() == "beer");
// 
// 	auto item = system.get(e);
// 	*item->data1 += 13;
// 	REQUIRE(e->data1 == 20);
// 	e->data2->get() += " & pizza";
// 	REQUIRE(e->data2->get() == "beer & pizza");
// }
// 
// TEST_CASE("System Memory Allocation & Deallocation") {
// 	System system;
// 	Entity e1 = Entity(0, 0, &system);
// 	REQUIRE_FALSE(system.get(e1).has_value());
// 
// 	ComponentConfig<int, std::string> cc;
// 	cc.get<COMPONENT_01_FLAG>() = 1;
// 	cc.get<COMPONENT_02_FLAG>() = "orange";
// 	e1 = system.make<int, std::string>(cc);
// 	REQUIRE(e1.generation == 0);
// 	REQUIRE(e1.index == 0);
// 
// 	auto item = system.get(e1);
// 	REQUIRE(item.has_value());
// 	REQUIRE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data1 == 1);
// 	REQUIRE(item->data2->get() == "orange");
// 
// 	cc.get<COMPONENT_01_FLAG>() = 42;
// 	cc.get<COMPONENT_02_FLAG>() = "apple";
// 	auto e2 = system.make<int, std::string>(cc);
// 	REQUIRE(e2.generation == 0);
// 	REQUIRE(e2.index == 1);
// 
// 	item = system.get(e1);
// 	REQUIRE(item.has_value());
// 	REQUIRE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data1 == 1);
// 	REQUIRE(item->data2->get() == "orange");
// 
// 	item = system.get(e2);
// 	REQUIRE(item.has_value());
// 	REQUIRE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data1 == 42);
// 	REQUIRE(item->data2->get() == "apple");
// 
// 	system.erase(e1);
// 	REQUIRE(e1.generation == 0);
// 	REQUIRE(e1.index == 0);
// 
// 	item = system.get(e1);
// 	REQUIRE_FALSE(item.has_value());
// 
// 	item = system.get(e2);
// 	REQUIRE(item.has_value());
// 	REQUIRE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data1 == 42);
// 	REQUIRE(item->data2->get() == "apple");
// 
// 	cc.get<COMPONENT_01_FLAG>() = {};
// 	cc.get<COMPONENT_02_FLAG>() = "pepper";
// 	auto e3 = system.make(cc);
// 	REQUIRE(e3.generation == 1);
// 	REQUIRE(e3.index == 0);
// 
// 	item = system.get(e1);
// 	REQUIRE_FALSE(item.has_value());
// 
// 	item = system.get(e2);
// 	REQUIRE(item.has_value());
// 	REQUIRE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data1 == 42);
// 	REQUIRE(item->data2->get() == "apple");
// 
// 	item = system.get(e3);
// 	REQUIRE(item.has_value());
// 	REQUIRE_FALSE(item->data1.has_value());
// 	REQUIRE(item->data2.has_value());
// 	REQUIRE(item->data2->get() == "pepper");
// }
// 
