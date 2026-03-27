from pathlib import Path

required_conan_version = ">=2.0"

from conan import ConanFile
from conan.tools.files import copy, save, collect_libs

class ConanConfiguration(ConanFile):
    settings = "arch", "os", "compiler", "build_type"
    options = {"shared": [True, False], 'header_only': [True, False]}
    default_options = {"shared": False, 'header_only': True}

    def set_name(self):
        try:
            self.name = Path(__file__).parent.joinpath('name-version.txt').read_text().split(':')[0].strip()
        except Exception as e:
            self.output.error(e)
            raise e

    def set_version(self):
        try:
            self.version = Path(__file__).parent.joinpath('name-version.txt').read_text().split(':')[1].strip()
        except Exception as e:
            self.output.error(e)
            raise e

    def requirements(self):
        try:
            self.requires("cyclonedds-cxx/0.10.5", transitive_headers=True)
        except Exception as e:
            self.output.error(e)
            raise e

    def generate(self):
        try:
            filename: str = 'conan-packages.cmake'
            content: str = ''
            cmake_package_name_property: str = 'cmake_file_name'

            content += 'set("${PROJECT_NAME}_CONAN_PACKAGE_NAMES"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.ref.name}"\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.cpp_info.get_property(cmake_package_name_property, check_type=str)}" # {dep.ref.name}\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.ref.version}" # {dep.ref.name}\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"\n'
            for dep in self.dependencies.values():
                content += f'    "{Path(dep.package_folder).as_posix()}" # {dep.ref.name}\n'
            content += ')\n'

            save(self, filename, content)

            for dep in self.dependencies.values():
                for dir in dep.cpp_info.bindirs:
                    copy(self, pattern="*", src=dir, dst=Path(self.build_folder).joinpath("bin").as_posix())
        except Exception as e:
            self.output.error(e)
            raise e

    def package(self):
        try:
            build_folder_path: Path = Path(self.build_folder)
            package_folder_path: Path = Path(self.package_folder)

            src: str = build_folder_path.joinpath("include").as_posix()
            dst: str = package_folder_path.joinpath("include").as_posix()
            copy(self, pattern="*.*", src=src, dst=dst)

            src: str = build_folder_path.joinpath("cmake").as_posix()
            dst: str = package_folder_path.joinpath("cmake").as_posix()
            copy(self, pattern="*.*", src=src, dst=dst)

            src: str = build_folder_path.joinpath("lib").as_posix()
            dst: str = package_folder_path.joinpath("lib").as_posix()
            copy(self, pattern="*.*", src=src, dst=dst)

            src: str = build_folder_path.joinpath("bin").as_posix()
            dst: str = package_folder_path.joinpath("bin").as_posix()
            copy(self, pattern="*.*", src=src, dst=dst)
        except Exception as e:
            self.output.error(e)
            raise e

    def package_info(self):
        try:
            self.cpp_info.set_property("cmake_file_name", self.name)
            self.cpp_info.libs = collect_libs(self)
        except Exception as e:
            self.output.error(e)
            raise e

    def package_id(self):
        try:
            if self.info.options.get_safe('header_only'):
                self.info.clear()
        except Exception as e:
            self.output.error(e)
            raise e
