from conan import ConanFile

class GrpcDemoRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("fmt/10.2.1")
        self.requires("boost/1.82.0")
        # self.requires("catch2/3.4.0")
        self.requires("grpc/1.54.3")
        self.requires("zlib/1.2.13")
