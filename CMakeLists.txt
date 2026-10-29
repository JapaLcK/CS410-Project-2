cmake_minimum_required(VERSION 3.16)
project(FoodPantry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#all additional implementation files must be added here.
add_library(pantry_lib
    src/Distribution.cpp
    src/FoodItem.cpp
    src/InventoryService.cpp
)
target_include_directories(pantry_lib PUBLIC include)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(
        distribution_tests
        tests/test_distribution.cpp
    )

    target_link_libraries(
        distribution_tests
        PRIVATE pantry_lib GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(distribution_tests)
    add_executable(
    inventory_service_tests
    tests/test_InventoryService.cpp
)

target_link_libraries(
    inventory_service_tests
    PRIVATE pantry_lib GTest::gtest_main
)

gtest_discover_tests(inventory_service_tests)
endif()


#Add the following when src/main.cpp is added:
# add_executable(food_pantry src/main.cpp)
# target_link_libraries(food_pantry PRIVATE pantry_lib)