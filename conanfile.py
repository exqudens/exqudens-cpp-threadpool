from pathlib import Path

required_conan_version = ">=2.0"

from conan import ConanFile
from conan.tools.files import copy, collect_libs

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
