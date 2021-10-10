A simple Entity Component System focused on being 100% of what you need 80% of the time. An ECS is specialized memory allocator commonly used in video games. It organizes similar data next to each other in memory, making it much easier to get cache hits.

Compilation Instrucions:
* Call make
* If on Windows you'll need to follow these instructions first: https://wiki.archlinux.org/title/Installation_guide

Usage:
* Define components with a #define COMP_* or a component_list.txt, haven't decided yet
* Create a System at the start of your program, and use it to create entities.
* Access entity data via: *system[entity]->get<Position>()
* Iterate via: for (auto [pos, vel] : system.iter<Position, Velocity>()) { pos += vel; }

1.0 Release TODO:
* Finalize Interface:
	* Make entities hold a system reference?
		* this would enable end users to not pass around a system everywhere
		* would also make entites fat
		* hides the fact that entites need to reference a system, not sure if this is good or bad
	* Item changes
		* Not sure if Item needs to exsist
		* If it should exsist, it needs to be System::Item
* Consolidate static Get Helpers
* Optimizations:
	* constexpr where I can
	* inlining
	* dense pack all data
	* move constructors and all that
* Multi-Threading:
	* Library Should be thread-safe
	* Integrate multithreading into System?
* Read & Write System and ComponentConfigs to file
* Useful README:
	* Write dependencies section in README
	* Update usage section in README
		* add code snippets
* Expand Testing (when interface is stable-ish):
	* More Unit Tests
	* Add Property Based Tests
	* Automatic Test Case Generation?
