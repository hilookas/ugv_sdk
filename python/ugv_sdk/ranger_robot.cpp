/*
 * ranger_robot.cpp
 *
 * Created on 14/5/2024 2:13 PM
 * Description: Python bindings for handling Ranger
 *
 * Copyright (c) 2024 Weston Robot Pte. Ltd.
 */
#include <pybind11/pybind11.h>

#include "agilex_message.hpp"
#include "ugv_sdk/mobile_robot/ranger_robot.hpp"

namespace py = pybind11;

using namespace westonrobot;

// clang-format off
PYBIND11_MODULE(ranger_robot, m) {
    m.doc() = "Python bindings for handling a Ranger robot";

    BindProtocolVersion(m);
    BindSystemStateMessage(m);
    BindMotionStateMessage(m);
    BindLightStateMessage(m);
    BindRcStateMessage(m);
    BindActuatorHSStateMessage(m);
    BindActuatorLSStateMessage(m);
    BindBmsBasicMessage(m);

    py::class_<RangerCoreState>(m, "RangerCoreState")
        .def(py::init<>())
        .def_property("time_stamp",
            [](const RangerCoreState &s) {
                // Convert time_point to duration since epoch in milliseconds
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        s.time_stamp.time_since_epoch());
                return duration.count();
            },
            [](RangerCoreState &s, long long ms) {
                // Convert milliseconds back to time_point
                s.time_stamp = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(ms));
            })
        .def_readwrite("system_state", &RangerCoreState::system_state)
        .def_readwrite("motion_state", &RangerCoreState::motion_state)
        .def_readwrite("light_state", &RangerCoreState::light_state)
        .def_readwrite("motion_mode_state", &RangerCoreState::motion_mode_state)
        .def_readwrite("rc_state", &RangerCoreState::rc_state)
        .def_readwrite("odometry", &RangerCoreState::odometry);

    py::class_<RangerActuatorState>(m, "RangerActuatorState")
        .def(py::init<>())
        .def_property("time_stamp",
            [](const RangerActuatorState &s) {
                // Convert time_point to duration since epoch in milliseconds
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        s.time_stamp.time_since_epoch());
                return duration.count();
            },
            [](RangerActuatorState &s, long long ms) {
                // Convert milliseconds back to time_point
                s.time_stamp = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(ms));
            })
        .def_readwrite("motor_angles", &RangerActuatorState::motor_angles)
        .def_readwrite("motor_speeds", &RangerActuatorState::motor_speeds)
        .def_property("actuator_hs_state",
            [](const RangerActuatorState &s) {
                // Convert C-style array to Python list
                return py::list(py::make_iterator(std::begin(s.actuator_hs_state), std::end(s.actuator_hs_state)));
            },
            [](RangerActuatorState &s, py::list l) {
                // Convert Python list to C-style array
                if (l.size() != 4) throw std::runtime_error("List size must be 4");
                for (size_t i = 0; i < 4; ++i) {
                    s.actuator_hs_state[i] = l[i].cast<ActuatorHSStateMessage>();
                }
            })
        .def_property("actuator_ls_state",
            [](const RangerActuatorState &s) {
                // Convert C-style array to Python list
                return py::list(py::make_iterator(std::begin(s.actuator_ls_state), std::end(s.actuator_ls_state)));
            },
            [](RangerActuatorState &s, py::list l) {
                // Convert Python list to C-style array
                if (l.size() != 4) throw std::runtime_error("List size must be 4");
                for (size_t i = 0; i < 4; ++i) {
                    s.actuator_ls_state[i] = l[i].cast<ActuatorLSStateMessage>();
                }
            });

    py::class_<RangerCommonSensorState>(m, "RangerCommonSensorState")
        .def(py::init<>())
        .def_property("time_stamp",
            [](const RangerCommonSensorState &s) {
                // Convert time_point to duration since epoch in milliseconds
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        s.time_stamp.time_since_epoch());
                return duration.count();
            },
            [](RangerCommonSensorState &s, long long ms) {
                // Convert milliseconds back to time_point
                s.time_stamp = std::chrono::steady_clock::time_point(
                        std::chrono::milliseconds(ms));
            })
        .def_readwrite("bms_basic_state", &RangerCommonSensorState::bms_basic_state);

    // RangerRobot class
    py::class_<RangerRobot>(m, "RangerRobot")
        .def(py::init<bool>(),
            py::arg("is_mini_v1") = false,
            "Constructor for RangerRobot with model version")
        .def("connect", &RangerRobot::Connect,
            py::arg("can_name"),
            "Connects the robot to the specified CAN interface.")
        .def("enable_commanded_mode", &RangerRobot::EnableCommandedMode,
            "To enable robot control")
        .def("request_version", &RangerRobot::RequestVersion,
            py::arg("timeout_sec") = 3,
            "Request the robot version")
        .def("reset_robot_state", &RangerRobot::ResetRobotState,
            "Reset the robot state")
        .def("disable_light_control", &RangerRobot::DisableLightControl,
            "Disable light control")
        .def("get_parser_protocol_version", &RangerRobot::GetParserProtocolVersion,
            "Get the parser protocol version")
        // robot control
        .def("set_motion_mode", &RangerRobot::SetMotionMode,
            py::arg("mode"),
            "Set the motion mode of the robot")
        .def("set_motion_command", &RangerRobot::SetMotionCommand,
            py::arg("linear_vel"),
            py::arg("steer_angle"),
            py::arg("angular_vel") = 0.0,
            "Set the motion command of the robot")
        .def("set_light_command", &RangerRobot::SetLightCommand,
            py::arg("f_mode"),
            py::arg("f_value"),
            py::arg("r_mode"),
            py::arg("r_value"),
            "Set the light command of the robot")
        // get robot state
        .def("get_robot_state", &RangerRobot::GetRobotState,
            "Get the robot state")
        .def("get_actuator_state", &RangerRobot::GetActuatorState,
            "Get the actuator state")
        .def("get_common_sensor_state", &RangerRobot::GetCommonSensorState,
            "Get the common sensor state");
}
// clang-format on