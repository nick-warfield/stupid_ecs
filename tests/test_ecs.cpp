#include <iostream>
#include "catch.hpp"
#include "ecs.hpp"

TEST_CASE("Component Config", "[component]") {
	ComponentConfig<int, std::string, char> cc;
	REQUIRE_FALSE(cc.get<COMPONENT_01_FLAG>().has_value());
	REQUIRE_FALSE(cc.get<COMPONENT_02_FLAG>().has_value());
	REQUIRE_FALSE(cc.get<8>().has_value());

	cc.component = 7;
	cc.remainder.component = "word";
	cc.remainder.remainder.component = 'y';

	REQUIRE(cc.get<COMPONENT_01_FLAG>().has_value());
	REQUIRE(cc.component == 7);
	REQUIRE(cc.component == cc.get<2>());
	REQUIRE(cc.component == cc.get<COMPONENT_01_FLAG>());

	REQUIRE(cc.get<COMPONENT_02_FLAG>().has_value());
	REQUIRE(cc.remainder.component == "word");
	REQUIRE(cc.remainder.component == cc.get<4>());
	REQUIRE(cc.remainder.component == cc.get<COMPONENT_02_FLAG>());

	REQUIRE(cc.get<8>().has_value());
	REQUIRE(cc.remainder.remainder.component == 'y');
	REQUIRE(cc.remainder.remainder.component == cc.get<8>());
}

// TEST_CASE("Entity dereferencing") {
// 	System system;
// 	ComponentConfig cc;
// 	cc.data1 = 7;
// 	cc.data2 = "word";
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
// 	ComponentConfig cc;
// 	cc.data1 = 5;
// 	cc.data2 = "pizza";
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
// 	ComponentConfig cc;
// 	cc.data1 = 1;
// 	cc.data2 = "orange";
// 	e1 = system.make(cc);
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
// 	cc.data1 = 42;
// 	cc.data2 = "apple";
// 	auto e2 = system.make(cc);
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
// 	cc.data1 = {};
// 	cc.data2 = "pepper";
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
