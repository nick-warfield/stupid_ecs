A simple Entity Component System focused on being 100% of what you need 80% of the time. An ECS is specialized memory allocator commonly used in video games. It organizes similar data next to each other in memory, making it much easier to get cache hits.

Compilation Instrucions:
* Call make
* If on Windows you'll need to follow these instructions first: https://wiki.archlinux.org/title/Installation_guide

Usage:
* Define components with a #define COMP_* or a component_list.txt, haven't decided yet
* Create a System at the start of your program, and use it to create entities.
* Access entity data via: entity->position.x;

TODO:
* Make entities hold a system reference?
	* this would enable end users to not pass around a system everywhere
	* would also make entites fat
	* hides the fact that entites need to reference a system, not sure if this is good or bad
* Implement system overloads and getters
	* system.get<Position>(entity) == system.get(entity)->get<Position>()
	* does system<Position>[entity] make sense?
* Item changes
	* Not sure if Item needs to exsist
	* If it should exsist, it needs to be System::Item
* Consolidate static Get Helpers
* constexpr and other optimizations
* User defined max number of components
	* prolly need some #ifdefs to accomplish this
* Verify vector + default allocator actually alligns data in memory
	* I'm pretty sure it does, but I need to make sure
	* If it doesn't, I guess I'll have to write an allocator lol
* Thread safety
* System::Iterator
* Read & Write System and ComponentConfigs to file
