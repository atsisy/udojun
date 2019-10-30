cmake_minimum_required(VERSION 3.10)

project(udjn_race)

set(udjn_src
  src/main.cpp
  src/character.cpp
  src/gm.cpp
  src/scene.cpp
  src/component.cpp
  src/move_func.cpp
  src/macro.cpp
  src/programable.cpp
  src/textures.cpp
  src/laser.cpp
  src/sched.cpp
  src/advanced_component.cpp
  src/effect.cpp
  src/title_scene.cpp
  src/keyboard.cpp
  src/fonts.cpp
  src/scene_animation.cpp
  src/episode_scene.cpp
  src/game_system.cpp
  src/bullet_management.cpp
  src/sub_scene.cpp
  src/dependent_util_function.cpp
  src/rotate.cpp
  src/geometry.cpp
  src/3d.cpp
  src/danmaku_data_structure.cpp
  src/enemy_management.cpp
  src/sound.cpp
  src/save_scene.cpp
  src/ranking_scene.cpp
  src/builtin_enemy.cpp
  src/credit_scene.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_definitions("-Wall -std=c++17 -O3 -march=native -I/home/takai/git-repo/sfml_test/include")
link_libraries("-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio")
add_executable(udjn_race ${udjn_src})
