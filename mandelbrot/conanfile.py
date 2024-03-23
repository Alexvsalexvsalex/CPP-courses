from conans import ConanFile, CMake, tools


class Conan(ConanFile):
    description = "HW1"
    settings = "os", "build_type", "arch", "compiler"
    generators = "cmake"
    exports_sources = "*"
    requires = [
        "boost/1.77.0",
        "pngpp/0.2.10"
    ]

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def deploy(self):
        self.copy("*", src="bin", dst="bin", keep_path=False)
