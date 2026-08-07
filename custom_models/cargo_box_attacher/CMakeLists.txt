# Copyright (c) 2017, United States Government, as represented by the
# Administrator of the National Aeronautics and Space Administration.
#
# All rights reserved.
#
# The Astrobee platform is licensed under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with the
# License. You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 3.0)
project(cargo_box_attacher)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  gazebo_ros
  message_generation
)

find_package(gazebo REQUIRED)

add_service_files(FILES
  Attach.srv
)

generate_messages()

catkin_package(
  LIBRARIES
    gazebo_world_plugin_cargo_attacher
  CATKIN_DEPENDS
    roscpp
    gazebo_ros
    message_runtime
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS}
)

link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

add_library(gazebo_world_plugin_cargo_attacher
  src/gazebo_world_plugin_cargo_attacher.cc
)
add_dependencies(gazebo_world_plugin_cargo_attacher
  ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(gazebo_world_plugin_cargo_attacher
  ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})

install(TARGETS gazebo_world_plugin_cargo_attacher
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_GLOBAL_LIB_DESTINATION}
)
