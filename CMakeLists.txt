cmake_minimum_required(VERSION 3.16)
project(WSysMon)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig)
include(FetchContent)

find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
        FetchContent_Declare(
                spdlog
                GIT_REPOSITORY https://github.com/gabime/spdlog.git
                GIT_TAG v1.10.0
        )
        FetchContent_MakeAvailable(spdlog)
endif()

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
find_package(X11 REQUIRED)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(deps ${GTKMM_INCLUDE_DIRS} ${X11_INCLUDE_DIR})

if(APPLE)
    add_compile_definitions(APPLE)
    enable_language(OBJCXX)

    set(CMAKE_EXE_LINKER_FLAGS "-framework Cocoa -framework Foundation")

    set(OS_SRC src/api/macos/DiskApi.cpp src/api/macos/GPUApi.cpp src/api/macos/HwMonApi.cpp src/api/macos/NetworkApi.cpp src/api/macos/ProcessesApi.cpp src/api/macos/SystemInfoApi.cpp src/api/macos/AutostartApi.cpp src/api/macos/ServicesApi.cpp src/utils/macos/MacUtils.mm)
elseif(UNIX)
    add_compile_definitions(LINUX)
    pkg_check_modules(SDBUS REQUIRED libsystemd)

    set(OS_SRC src/api/linux/ProcessesApi.cpp src/api/linux/GPUApi.cpp src/api/linux/NetworkApi.cpp src/api/linux/SystemInfoApi.cpp src/api/linux/DiskApi.cpp src/api/linux/gpu/nvml.h src/api/linux/gpu/nvml_hook_funcs.inl src/api/linux/gpu/NvGpuApi.cpp src/api/linux/gpu/NvGpuApi.h src/api/linux/HwMonApi.cpp src/api/linux/gpu/DrmGpuApi.cpp src/api/linux/gpu/DrmGpuApi.h src/api/linux/AutostartApi.cpp src/api/linux/ServicesApi.cpp)
endif()

add_executable(WSysMon src/main.cpp src/ui/MainWindow.cpp src/ui/widgets/GraphWidget.cpp src/ui/widgets/PerformanceButton.cpp src/ui/views/ProcessesView.cpp src/ui/views/PerformanceView.cpp src/ui/views/performance/PerformanceCPUView.cpp src/ui/views/performance/PerformanceGPUView.cpp src/ui/views/performance/PerformanceNetworkView.cpp src/ui/views/performance/PerformanceRAMView.cpp src/utils/DispatcherThread.cpp src/utils/UnitConverter.cpp src/utils/linux/X11Utils.cpp src/api/process/ProcessManager.cpp src/api/process/ProcessNode.cpp src/api/DiskApi.h src/ui/views/performance/PerformanceDiskView.cpp src/ui/views/performance/PerformanceDiskView.h src/api/HwMonApi.h src/storage/AppSettings.cpp src/storage/AppSettings.h src/ui/views/AutostartView.cpp src/ui/views/AutostartView.h src/api/AutostartApi.h src/ui/DetailsWindow.cpp src/ui/DetailsWindow.h src/ui/views/performance/PerformancePSUView.cpp src/ui/views/performance/PerformancePSUView.h src/api/ServicesApi.h ${OS_SRC} src/utils/linux/CGUtils.cpp src/utils/linux/CGUtils.h src/ui/views/ServicesView.cpp src/ui/views/ServicesView.h)
target_link_libraries(WSysMon PRIVATE spdlog::spdlog pthread ${SDBUS_LIBRARIES} ${CMAKE_DL_LIBS} ${GTKMM_LIBRARIES} ${X11_LIBRARIES})
