cmake_minimum_required(VERSION 3.16)
project(WSysMon)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig)
include(FetchContent)

find_package(spdlog)
if(NOT spdlog_FOUND)
        FetchContent_Declare(
                spdlog
                GIT_REPOSITORY https://github.com/gabime/spdlog.git
                GIT_TAG v1.10.0
        )
        FetchContent_MakeAvailable(spdlog)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.24.0")
    FetchContent_Declare(
            json
            URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    )
else()
    FetchContent_Declare(
            json
            URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
            DOWNLOAD_EXTRACT_TIMESTAMP true
    )
endif()
FetchContent_MakeAvailable(json)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
find_package(X11 REQUIRED)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS} ${X11_INCLUDE_DIR})

add_executable(WSysMon src/main.cpp src/ui/MainWindow.cpp src/ui/widgets/GraphWidget.cpp src/ui/widgets/PerformanceButton.cpp src/ui/views/ProcessesView.cpp src/ui/views/PerformanceView.cpp src/ui/views/performance/PerformanceCPUView.cpp src/ui/views/performance/PerformanceGPUView.cpp src/ui/views/performance/PerformanceNetworkView.cpp src/ui/views/performance/PerformanceRAMView.cpp src/utils/DispatcherThread.cpp src/utils/UnitConverter.cpp src/utils/X11Utils.cpp src/api/process/ProcessManager.cpp src/api/process/ProcessNode.cpp src/api/linux/ProcessesApi.cpp src/api/linux/GPUApi.cpp src/api/linux/NetworkApi.cpp src/api/linux/SystemInfoApi.cpp src/api/DiskApi.h src/api/linux/DiskApi.cpp src/ui/views/performance/PerformanceDiskView.cpp src/ui/views/performance/PerformanceDiskView.h src/api/linux/gpu/nvml.h src/api/linux/gpu/nvml_hook_funcs.inl src/api/linux/gpu/NvGpuApi.cpp src/api/linux/gpu/NvGpuApi.h src/api/HwMonApi.h src/api/linux/HwMonApi.cpp src/storage/AppSettings.cpp src/storage/AppSettings.h src/api/linux/gpu/DrmGpuApi.cpp src/api/linux/gpu/DrmGpuApi.h src/ui/views/AutostartView.cpp src/ui/views/AutostartView.h src/api/AutostartApi.h src/api/linux/AutostartApi.cpp)
target_link_libraries(WSysMon PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json pthread ${CMAKE_DL_LIBS} ${GTKMM_LIBRARIES} ${X11_LIBRARIES})
