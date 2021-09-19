#include "catch.hpp"
#include "ecs.hpp"

TEST_CASE("ECS") {
	System system;
	Entity e1 = Entity(0, 0);
	REQUIRE_FALSE(system.get(e1).has_value());

	ComponentConfig cc;
	cc.data1 = 1;
	cc.data2 = "orange";
	e1 = system.make(cc);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	auto item = system.get(e1);
	REQUIRE(item.has_value());
	REQUIRE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data1 == 1);
	REQUIRE(item->data2->get() == "orange");

	cc.data1 = 42;
	cc.data2 = "apple";
	auto e2 = system.make(cc);
	REQUIRE(e2.generation == 0);
	REQUIRE(e2.index == 1);

	item = system.get(e1);
	REQUIRE(item.has_value());
	REQUIRE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data1 == 1);
	REQUIRE(item->data2->get() == "orange");

	item = system.get(e2);
	REQUIRE(item.has_value());
	REQUIRE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data1 == 42);
	REQUIRE(item->data2->get() == "apple");

	system.erase(e1);
	REQUIRE(e1.generation == 0);
	REQUIRE(e1.index == 0);

	item = system.get(e1);
	REQUIRE_FALSE(item.has_value());

	item = system.get(e2);
	REQUIRE(item.has_value());
	REQUIRE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data1 == 42);
	REQUIRE(item->data2->get() == "apple");

	cc.data1 = {};
	cc.data2 = "pepper";
	auto e3 = system.make(cc);
	REQUIRE(e3.generation == 1);
	REQUIRE(e3.index == 0);

	item = system.get(e1);
	REQUIRE_FALSE(item.has_value());

	item = system.get(e2);
	REQUIRE(item.has_value());
	REQUIRE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data1 == 42);
	REQUIRE(item->data2->get() == "apple");

	item = system.get(e3);
	REQUIRE(item.has_value());
	REQUIRE_FALSE(item->data1.has_value());
	REQUIRE(item->data2.has_value());
	REQUIRE(item->data2->get() == "pepper");
}

