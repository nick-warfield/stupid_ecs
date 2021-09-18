A simple Entity Component System focused on being 100% of what you need 80% of the time. An ECS is specialized memory allocator commonly used in video games. It organizes similar data next to each other in memory, making it much easier to get cache hits.

Compilation Instrucions:
* Call make
* If on Windows you'll need to follow these instructions first: https://wiki.archlinux.org/title/Installation_guide

Usage:
* Define components with a #define COMP_* or a component_list.txt, haven't decided yet
* Create a System at the start of your program, and use it to create entities.
* Access entity data via: entity->position.x;
