A simple Entity Component System focused on being 100% of what you need 80% of the time. An ECS is specialized memory allocator commonly used in video games. It organizes similar data next to each other in memory, making it much easier to get cache hits.

Compilation Instrucions:
* Call make
* If on Windows you'll need to follow these instructions first: https://wiki.archlinux.org/title/Installation_guide

Usage:
* Define components with a #define COMP_* or a component_list.txt, haven't decided yet
* Create a System at the start of your program, and use it to create entities.
* Access entity data via: entity->position.x;

TODO:
* wrap everything in a namespace
* switch to boost::optional
	* std::optional<std::reference_wrapper<T>> is clunky, both for me and end users
	* I could return T*const, but optional<T&> is much more clear about the intent
* Make entities hold a system reference?
	* this would enable end users to not pass around a system everywhere
	* would also make entites fat
	* hides the fact that entites need to reference a system, not sure if this is good or bad
* Implement system overloads and getters
	* system[entity] instead of system.get(entity)
	* system.get<Position>(entity) == system.get(entity)->get<Position>()
	* does system<Position>[entity] make sense?
* Item changes
	* Not sure if Item needs to exsist
	* If it should exsist, it needs to be System::Item
* Consolidate static Get Helpers
* Restrict Entity creation to System
* User defined max number of components
	* prolly need some #ifdefs to accomplish this
* Thread safety
* System::Iterator
* Read & Write System and ComponentConfigs to file
