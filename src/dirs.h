#ifndef DIRS_H_
#define DIRS_H_
#include <string>

namespace tetriskl {
    class ResourceLocator {
    private:
        std::string asset_dir;
        std::string storage_dir;
    public:
        ResourceLocator(int argc, const char *argv[]);

        std::string get_asset_path(std::string path);
        std::string get_storage_path(std::string path);
    };
}

#endif // DIRS_H_
