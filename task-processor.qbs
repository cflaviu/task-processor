import qbs

CppApplication {
    consoleApplication: true
    cpp.cxxLanguageVersion: "c++20"
    cpp.discardUnusedData: true
    cpp.staticLibraries: [
        "pthread",
    ]
    cpp.includePaths: [
        "."
    ]
    files: [
        "main.cpp",
        "task/queue/blocking.hpp",
        "task/processor/io.hpp",
        "task/processor/multi.hpp",
        "task/processor/basic.hpp",
        "task/processor/base.hpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
