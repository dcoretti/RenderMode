# RenderMode
GL renderer with a focus on memory abstraction layers.  

# Features
* Pooled memory arenas with separate allocator types.
* "Handle" system for memory abstraction with two degrees of separation: handle with version -> internal index with versioning -> actual memory.  Versioning supported on handles to enable visibility into possible memory leaks when handles point to no longer existing memory.
* Basic render command system.  Render commands separate rendering tasks from the graphics library layer.  Commands utilize memory arenas to cleanly manage memory between frames.  Lists of commands enable easy sorting to group shared draw commands.
* Basic (earlier version) of text rendering library.
* Basic obj model loading for static geometry.
