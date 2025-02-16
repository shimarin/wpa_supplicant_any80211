#include <unistd.h>

#include <print>
#include <filesystem>
#include <argparse/argparse.hpp>

std::optional<std::string> find_interface()
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
    return std::nullopt;
}

std::string wait_for_interface(int interval = 5)
{
    while (true) {
        auto interface = find_interface();
        if (interface) {
            return *interface;
        }
        //else
        sleep(interval);
    }
}

void run(const std::filesystem::path& config, int interval, bool dry_run)
{
    auto interface = interval > 0? wait_for_interface() : find_interface();
    if (!interface) {
        throw std::runtime_error("No WiFi interface found");
    }

    if (dry_run) {
        std::println("wpa_supplicant -i{} -c{}", *interface , config.string());
        return;
    }

    // else start wpa_supplicant
    std::println("Using interface: {}", *interface);
    auto rst = execlp("wpa_supplicant", "wpa_supplicant", 
        ("-i" + *interface).c_str(), ("-c" + config.string()).c_str(), nullptr);
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
    program.add_argument("--wait", "-w")
        .help("Wait interval for a WiFi interface to become available.")
        .implicit_value(5);
//        .scan<'i', int>();
    program.add_argument("--dry-run")
        .help("Print the interface name/command line and exit")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::println("{}", err.what());
        std::cout << program;
        return 1;
    }

    auto wait = program.present<int>("--wait");
    if (wait) {
        if (*wait <= 0) {
            std::println(std::cerr, "Invalid wait interval");
            return 1;
        }
        //else
        std::println("Waiting for a WiFi interface to become available(Interval {} sec)", *wait);
    }

    try {
        run(program.get<std::string>("config"), 
            wait.value_or(0),
            program.get<bool>("--dry-run"));
    } catch (const std::exception& err) {
        std::println(std::cerr, "{}", err.what());
        return 1;
    }

    return 0;
}