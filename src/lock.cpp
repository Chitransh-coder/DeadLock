#include "../include/deadlock.hpp"
#include <iostream>
#include <vector>
#include "../include/structs.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace std;

// Resolved error in initializing pybind11
#ifndef PyEval_ThreadsInitialized()
#define PyEval_InitThreads()
#endif

PYBIND11_MODULE(_deadlock, d)
{
    d.doc() = "DeadLock - A Project Manager for Data Scientists";

    py::class_<DeadLock>(d, "deadlock")
        .def(py::init<>())
        // Create Project
        .def("create_project", &DeadLock::init)
        // Install Packages
        .def("install_packages", &DeadLock::installPackages)
        // Sync from DeadLock
        .def("sync_packages", &DeadLock::syncFromDeadLock)
        // Get Package Info
        .def("get_info", &DeadLock::getPackageInfo)
        // Is Package Installed
        .def("is_installed", &DeadLock::isPackageInstalled)
        // Uninstall Packages
        .def("uninstall_packages", &DeadLock::uninstallPackages);
}
