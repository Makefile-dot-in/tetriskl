#include "dirs.h"
#include <stdexcept>

namespace tetriskl {
#ifdef WIN32
    constexpr char PATH_SEPARATOR = '\\';
#else
    constexpr char PATH_SEPARATOR = '/';
#endif

    std::string get_path_stem(std::string path) {
        std::string::size_type idx = path.rfind(PATH_SEPARATOR);
        if (idx == std::string::npos) return "";

        return std::string(std::move(path), 0, idx);
    }

    ResourceLocator::ResourceLocator(int argc, const char *argv[]) {
        if (argc < 1)
            throw std::out_of_range("didn't receive the program name");

        std::string program_path(argv[0]);
        std::string program_dir = get_path_stem(program_path);
        asset_dir = program_dir + PATH_SEPARATOR + ".." + PATH_SEPARATOR + "assets";
        storage_dir = program_dir + PATH_SEPARATOR + ".." + PATH_SEPARATOR + "storage";
    }

    std::string ResourceLocator::get_asset_path(std::string path) {
        return asset_dir + PATH_SEPARATOR + path;
    }

    std::string ResourceLocator::get_storage_path(std::string path) {
        return storage_dir + PATH_SEPARATOR + path;
    }
}
