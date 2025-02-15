# wpa_supplicant_any80211

`wpa_supplicant_any80211` is a C++ program that automatically detects the first WiFi interface on the system and starts `wpa_supplicant` using the specified configuration file.

## Requirements

- C++23 compatible compiler (e.g., g++)
- [argparse](https://github.com/p-ranav/argparse) library

## Installation

### Build from Source

1. Clone the repository:

```sh
git clone https://github.com/yourusername/wpa_supplicant_any80211.git
cd wpa_supplicant_any80211
```
 
2. Build the program:

```sh
make
```
    
3. Install the program:

```sh
sudo make install
```
    
## Usage

wpa_supplicant_any80211 starts `wpa_supplicant` using the specified configuration file.

```sh
./wpa_supplicant_any80211 [config] [--dry-run]
```
