project(udjn_race)

set(udjn_src
  src/main.cpp
  src/character.cpp
  src/gm.cpp
  src/scene.cpp
  src/component.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions("-Wall -std=c++14 -I/home/takai/Dropbox/programs/sfml_test/include")
link_libraries("-lsfml-graphics -lsfml-window -lsfml-system")
add_executable(udjn_race ${udjn_src})
