#include <unistd.h>

#include <filesystem>
#include <argparse/argparse.hpp>

std::string find_interface()
{
    const std::filesystem::path sysfs("/sys/class/net");
    // get the first network interface which has "phy80211" subdirectory
    for (const auto& entry : std::filesystem::directory_iterator(sysfs)) {
        const auto& path = entry.path();
        if (std::filesystem::is_directory(path / "phy80211")) {
            return path.filename().string();
        }
    }
    // not found
    throw std::runtime_error("No applicable WiFi interface found");
}

void run(const std::filesystem::path& config, bool dry_run)
{
    auto interface = find_interface();

    if (dry_run) {
        std::cout << "wpa_supplicant -i" << interface 
            << " -c" << config.string() << std::endl;
        return;
    }

    // else start wpa_supplicant
    std::cout << "Using interface: " << interface << std::endl;
    auto rst = execlp("wpa_supplicant", "wpa_supplicant", 
        ("-i" + interface).c_str(), ("-c" + config.string()).c_str(), nullptr);
    if (rst < 0) {
        throw std::runtime_error("Failed to start wpa_supplicant");
    }
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("wpa_supplicant_any80211");
    program.add_argument("config")
        .help("Interface to use")
        .default_value("/etc/wpa_supplicant/wpa_supplicant.conf");
    program.add_argument("--dry-run")
        .help("Print the interface name/command line and exit")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        return 1;
    }

    try {
        run(program.get<std::string>("config"), program.get<bool>("--dry-run"));
    } catch (const std::exception& err) {
        std::cout << err.what() << std::endl;
        return 1;
    }

    return 0;
}