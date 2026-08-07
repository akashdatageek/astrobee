/* Copyright (c) 2017, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 *
 * All rights reserved.
 *
 * The Astrobee platform is licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

// ROS includes
#include <ros/ros.h>

// Gazebo includes
#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>

// Service message
#include <cargo_box_attacher/Attach.h>

// STL includes
#include <map>
#include <string>

namespace gazebo {

/* The Astrobee perching arm gripper is position-controlled only: it has
   no grasp detection or contact constraint, so closing it on a free
   body penetrates or ejects the body. This plugin provides the fixed
   joint that a successful grasp would create. The caller decides when a
   pick succeeds and requests a weld between a gripper link and a cargo
   box link; a detach request releases it again. */
class WorldPluginCargoAttacher : public WorldPlugin {
 public:
  // Constructor
  WorldPluginCargoAttacher() {}

  // Destructor
  ~WorldPluginCargoAttacher() {}

  void Load(physics::WorldPtr world, sdf::ElementPtr sdf) {
    world_ = world;
    // Check ROS is initialized
    if (!ros::isInitialized()) {
      ROS_FATAL_STREAM("ROS has not been initialized");
      return;
    }
    nh_ = ros::NodeHandle("/");
    srv_attach_ = nh_.advertiseService("/sim/cargo/attach",
      &WorldPluginCargoAttacher::AttachCallback, this);
    srv_detach_ = nh_.advertiseService("/sim/cargo/detach",
      &WorldPluginCargoAttacher::DetachCallback, this);
    gzmsg << "Cargo attacher ready on /sim/cargo/attach and "
          << "/sim/cargo/detach" << std::endl;
  }

 private:
  // Unique key (and joint name) for an attachment request
  std::string Key(cargo_box_attacher::Attach::Request const& req) {
    return req.model_1 + "_" + req.link_1 + "_"
         + req.model_2 + "_" + req.link_2 + "_joint";
  }

  // Find a link by model and link name, setting an error on failure
  physics::LinkPtr FindLink(std::string const& model,
    std::string const& link, std::string *error) {
    #if GAZEBO_MAJOR_VERSION > 7
    physics::ModelPtr m = world_->ModelByName(model);
    #else
    physics::ModelPtr m = world_->GetModel(model);
    #endif
    if (!m) {
      *error = "Model not found: " + model;
      return physics::LinkPtr();
    }
    physics::LinkPtr l = m->GetLink(link);
    if (!l) {
      *error = "Link not found: " + model + "::" + link;
      return physics::LinkPtr();
    }
    return l;
  }

  // Weld the two requested links together with a fixed joint
  bool AttachCallback(cargo_box_attacher::Attach::Request &req,
    cargo_box_attacher::Attach::Response &res) {
    std::string key = Key(req);
    if (joints_.count(key) > 0) {
      res.ok = true;
      res.message = "Already attached: " + key;
      return true;
    }
    std::string error;
    physics::LinkPtr parent = FindLink(req.model_1, req.link_1, &error);
    physics::LinkPtr child = FindLink(req.model_2, req.link_2, &error);
    if (!parent || !child) {
      res.ok = false;
      res.message = error;
      return true;
    }
    #if GAZEBO_MAJOR_VERSION > 7
    physics::PhysicsEnginePtr engine = world_->Physics();
    #else
    physics::PhysicsEnginePtr engine = world_->GetPhysicsEngine();
    #endif
    physics::JointPtr joint = engine->CreateJoint("fixed",
      parent->GetModel());
    joint->Attach(parent, child);
    joint->Load(parent, child, ignition::math::Pose3d());
    joint->SetName(key);
    joint->Init();
    joints_[key] = joint;
    res.ok = true;
    res.message = "Attached: " + key;
    return true;
  }

  // Release a weld previously created by AttachCallback
  bool DetachCallback(cargo_box_attacher::Attach::Request &req,
    cargo_box_attacher::Attach::Response &res) {
    std::string key = Key(req);
    std::map<std::string, physics::JointPtr>::iterator it =
      joints_.find(key);
    if (it == joints_.end()) {
      res.ok = false;
      res.message = "Not attached: " + key;
      return true;
    }
    it->second->Detach();
    joints_.erase(it);
    res.ok = true;
    res.message = "Detached: " + key;
    return true;
  }

  ros::NodeHandle nh_;
  ros::ServiceServer srv_attach_;
  ros::ServiceServer srv_detach_;
  physics::WorldPtr world_;
  std::map<std::string, physics::JointPtr> joints_;
};

// Register this plugin with the simulator
GZ_REGISTER_WORLD_PLUGIN(WorldPluginCargoAttacher)

}   // namespace gazebo
