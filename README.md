# Team 02

![cmake workflow](https://github.com/nus-cs3203/21s1-cp-spa-team-02//actions/workflows/cmake.yml/badge.svg)

## Target Environment

Item | Version
-|-
OS | Windows 10
Toolchain | Windows Visual Studio (CMake Project)
C++ Standard | C++17

### Additional Build Instructions

Name of solution: autotester
Path to executable produced: Team02/Code02/build_win/x86-Release/src/autotester/autotester

1. Run and build using VS2019 in Release configuration.

2. Open the given Team02 folder in VS2019. Visual Studio automatically generates the CMake cache and detects targets specified in `CMakeLists.txt`.

3. Build All files in Visual Studio, with the x86-Release configuration.

4. After the executable have been built, navigate to the `Team02/Code02/tests` directory and run the following command to run the autotester executable: `../build_win/x86-Release/src/autotester/autotester Sample_source.txt Sample_queries.txt out.xml`

5. Similarly, in order to run our system tests, navigate to the `Team02/Tests02` directory and run the following command: `../Code02/build_win/x86-Release/src/autotester/autotester AmbiguousWhitespace_source.txt AmbiguousWhitespace_queries.txt AmbiguousWhitespace_out.xml`. Repeat for all files in the folder.

Note: Our three SIMPLE programs that satisfy the system acceptance test case requirements are named `FunctionalTest1_source.txt`, `FunctionalTest2_source.txt`, and `FunctionalTest3_source.txt`.

# Team Members

Name | Mobile | Email | Development OS/Toolchain
-:|:-:|:-|-|
Keith Low Jun | 90174454 | e0406245@u.nus.edu | Windows 10/Visual Studio 2019
Lim En Sheng Merlin | 98314003 | e0407100@u.nus.edu | Windows 10/Visual Studio 2019
Ng Choon Kang Warren | 90031727 | warren.ng@u.nus.edu | Windows 10/Visual Studio 2019
Tang Guofeng | 86789700 | guofeng.tang@u.nus.edu | MacOS/CLion
Tay Yee Ting Beatrice | 91383530 | e0325690@u.nus.edu | MacOS/CLion
Wong Chi Shan | 97785582 | wongchishan@u.nus.edu | Windows 10/Visual Studio 2019
