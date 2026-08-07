\page cargobox Cargo Box

This package provides a pickable cargo box model for the Astrobee ISS
simulation. The box is a 0.20 m yellow cube with a mass of 1.0 kg, using
primitive box geometry for both visual and collision (mesh collisions are slow
and jittery in ODE).

Five instances of this model are placed in the JPM interior by
`simulation/worlds/iss.world`, named `cargo_box_1` through `cargo_box_5`.

## Physics tuning

The ISS world sets gravity to zero, so a low-mass free body is easily flung
across the module by a gripper contact with default parameters. The model
therefore sets:

- `mu1` / `mu2` = 1.5 — high friction so contacts grip instead of slide
- `kp` = 100000, `kd` = 10 — contact stiffness and damping
- `minDepth` = 0.001, `maxVel` = 0.1 — limit contact correction velocity
- `velocity_decay` 0.05 (linear and angular) — boxes drift to a stop instead
  of coasting forever in zero-g

## Verifying the package is discoverable

Gazebo resolves `model://astrobee_cargo_box` against `GAZEBO_MODEL_PATH`,
which `gazebo_ros` populates from the `<export>` block in `package.xml`.
After building:

    catkin build astrobee_cargo_box
    source devel/setup.bash
    echo $GAZEBO_MODEL_PATH | tr ':' '\n' | grep -i cargo

The grep must return a path. If it does not, the workspace was not re-sourced
or the `<export>` block is broken.

## Spawning a box manually

    rosrun gazebo_ros spawn_model \
      -file `rospack find astrobee_cargo_box`/urdf/model.urdf \
      -urdf -model cargo_box_test -x 10.9 -y -8.6 -z 3.95

## Mapper interaction

The QP planner plans against an octomap built by `mobility/mapper` from the
haz cam point cloud. In this release the mapper is **disabled by default**
(`disable_mapper = true` in `astrobee/config/mobility/mapper.config`), so the
planner is blind to the cargo boxes and will happily plan straight through
them. If the mapper is re-enabled (it is reconfigurable at runtime), the
planner will see the boxes and may reject routes it previously accepted. The
pick-and-place demo assumes the default (mapper off). To check which mode a
running sim is in:

    rostopic list | grep mapper
    rostopic echo /mob/mapper/free_space_cloud -n 1

Grasping is provided by the \ref cargoboxattacher package.
