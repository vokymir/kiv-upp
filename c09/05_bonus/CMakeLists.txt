CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(upp_mpi_bonus)

SET(CMAKE_CXX_STANDARD 20)

# NOTE: toto neni uplne nejlepsi praktika, lepsi je nastavit bud promennou prostredi CC a CXX
# nebo do CMake prikazu tyto konstanty predat
# Pro potreby testovani vsak tohle ale muzeme normalne pouzit:
#SET(CMAKE_C_COMPILER "mpicc")
#SET(CMAKE_CXX_COMPILER "mpicxx")

FILE(GLOB_RECURSE sources src/*.cpp src/*.c src/*.hpp src/*.h)

find_package(MPI REQUIRED)

ADD_EXECUTABLE(upp_mpi_bonus ${sources})

target_link_libraries(upp_mpi_bonus MPI::MPI_CXX)
