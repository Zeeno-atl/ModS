# ModS

Implementing the paradigm of modular architecture has many advantages:
 
 - Faster compile time, since you need to rebuild only one module
 - Explicit dependencies, since all the dependencies are resolved by modular system at runtime
 - Decoupling, since all the routing is done by the modular system
 - Generation of dependency graph
 - Changing implementations during runtime
 
There are of course also disadantages
 - You can loose track of other modules with big projects (this is disadvantage only if you need to have everything under control)
 
 
Architecture
===

Modular system consist of one base App that does all the wiring and routing and all the usefull code is enclosed in modules.

You basically only need to link your main app with the `mods` library and the module part is header-only.
