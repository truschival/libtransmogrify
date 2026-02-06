from conan import ConanFile
from conan.tools.files import update_conandata
from conan.tools.scm import Git
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import re


class LibtransmogrifyRecipe(ConanFile):
    name = "libtransmogrify"
    # version will be set dynamically by set_version() method
    git_describe_rev = "unknown"
    # Optional metadata
    license = "MIT"
    user = "truschival"
    author = "Thomas Ruschival <thomas@ruschival.de>"
    url = "github.com/truschival/libtransmogrify"
    description = "Playing around with Conan, CMake and GoogleTest"

    # Binary configuration
    settings = "compiler", "build_type", "arch", "os"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "BUILD_TEST": [True, False],
    }
    default_options = {"shared": True, "fPIC": True, "BUILD_TEST": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = [
        "cmake/*",
        "CMakeModules/*",
        "CMakeLists.txt",
        "LICENSE",
        "README.md",
        "src/*",
        "include/*",
    ]

    def set_version(self):
        version = self.get_semver_from_git()
        self.version = version

    def get_git_describe_rev(self):
        git = Git(self, self.recipe_folder)
        git_describe_rev = git.run(
            "describe --tags --long --abbrev=8 --always --dirty"
        ).strip()
        self.output.info(f"Git describe revision: {git_describe_rev}")
        return git_describe_rev

    def get_semver_from_git(self):
        git_describe_rev = self.get_git_describe_rev()

        pattern = r"^([A-Za-z]+)?([0-9]+)(\.[0-9]+)?(\.[0-9]+)?(\.[0-9]+)?(-.*)?"
        match = re.match(pattern, git_describe_rev)

        if not match:
            raise Exception(
                f"Cannot parse git describe output '{git_describe_rev}' to semver version!"
            )

        # Build version string - start with major version (group 2)
        version_parts = [match.group(2)]

        # Add minor, patch, and tweak if they exist (groups 3, 4, 5)
        for i in range(3, 6):
            part = match.group(i)
            if part:
                version_parts.append(part)
            else:
                break
        version = "".join(version_parts)

        return version

    def export(self):
        self.output.info(
            "Exporting recipe - updating conandata.yml with git describe revision"
        )
        git_describe_rev = self.get_git_describe_rev()
        update_conandata(self, {"git_describe_rev": git_describe_rev})

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        # Now run CMake normally - provide dummy variables to skip git detection
        cmake = CMake(self)
        cmake.configure(
            variables={
                "CONAN_PROJECT_VERSION": self.version,
                "CONAN_GIT_DESCRIBE_REV": self.conan_data.get(
                    "git_describe_rev", "XXX"
                ),
            }
        )
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # Handle debug postfix for library names
        lib_name = "transmogrify"
        if self.settings.build_type == "Debug":
            lib_name += "_dbg"

        # For shared libraries on Windows, we might need different handling
        # but for Unix systems, the base name is the same
        self.cpp_info.libs = [lib_name]

        # Export the minimal required compile definitions for Conan compatibility
        self.cpp_info.defines = ["FOO_BAR=1"]
