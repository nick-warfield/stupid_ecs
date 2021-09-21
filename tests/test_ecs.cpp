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

TEST_CASE("System Allocation") {
	System sys1;
	ComponentConfig cc1;

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

	System<int> sys2;
	ComponentConfig<int> cc2;

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

	System<char, std::string, int, float, std::vector<std::size_t>> sys3;
	ComponentConfig<char, std::string, int, float, std::vector<std::size_t>> cc3;

	e1 = sys3.make(cc3);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	e2 = sys3.make(cc3);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);

	e3 = sys3.make(cc3);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	sys3.erase(e2);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);

	e4 = sys3.make(cc3);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);
	REQUIRE(e3.generation == 0);
	REQUIRE(e3.index == 2);
	REQUIRE(e4.generation == 1);
	REQUIRE(e4.index == 1);
};

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
