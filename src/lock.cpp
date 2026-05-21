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

PYBIND11_MODULE(deadlock_core, d)
{
    d.doc() = "DeadLock - A Project Manager for Data Scientists";
    py::class_<Package>(d, "Package")
        .def(py::init<>())
        .def_readwrite("name", &Package::name)
        .def_readwrite("version", &Package::version)
        .def_readwrite("source", &Package::source)
        .def_readwrite("installDate", &Package::installDate)
        .def_readwrite("dependencies", &Package::dependencies)
        .def("__repr__", [](const Package &p)
             { return std::string("<Package ") + p.name + " " + p.version + ">"; });

    auto dl = py::class_<DeadLock>(d, "DL_Wrapper");
    dl
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
        .def("uninstall_packages", &DeadLock::uninstallPackages)
        // List installed packages
        .def("get_installed_packages", &DeadLock::getInstalledPackages);

    // Optional alias: make Package accessible as deadlock_core.DL_Wrapper.Package
    d.attr("DL_Wrapper").attr("Package") = d.attr("Package");
}
