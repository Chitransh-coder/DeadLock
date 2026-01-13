#include "../include/deadlock.h"
#include <iostream>
#include <vector>
#include "../include/structs.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace std;

PYBIND11_MODULE(_deadlock, d)
{
    d.doc() = "DeadLock - A Project Manager for Data Scientists";

    py::class_<DeadLock>(d, "DeadLock")
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
