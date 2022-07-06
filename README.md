A simple Entity Component System focused on being 100% of what you need 80% of the time. An ECS is specialized memory allocator commonly used in video games. It organizes similar data next to each other in memory, making it much easier to get cache hits.

Usage:
* Create a System at the start of your program, and use it to create entities.
* Access entity data via: *system[entity]->get<Position>()
* Iterate via: for (auto [pos, vel] : system.iter<Position, Velocity>()) { pos += vel; }

1.0 Release TODO:
* Finalize Interface:
	* System::export() - returns all living entities as a vector of components
	* Read & Write ComponentConfigs to file
	* Item changes
		* Not sure if Item needs to exsist
		* If it should exsist, it needs to be System::Item
	* Consolidate static Get Helpers
* Optimizations:
	* constexpr where I can
	* inlining
	* move semantics and all that
	* dense pack all data
	* nullopt configs should be uninitialized instead of using default constructor
	* arena allocation?
	* replace vector with arrays?
	* improve compile times
* Multi-Threading:
	* Library Should be thread-safe
	* Integrate multithreading into System?
* Useful README:
	* Write dependencies section in README
	* Update usage section in README
		* add code snippets
* Expand Testing (when interface is stable-ish):
	* More Unit Tests
	* Add Property Based Tests
	* Automatic Test Case Generation?
