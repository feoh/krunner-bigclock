# KRunner Bigclock

KRunner Bigclock is a KDE Frameworks 6 / Qt 6 KRunner plugin that displays a large 1980s-style LED digital clock in the center of the screen.

Invoke it from KRunner with one of:

- `bigclock`
- `big clock`
- `clock`
- `time`

The clock closes automatically after 30 seconds, or immediately when you press Escape/Enter or click it.

## Build

Install build dependencies first.

### Arch Linux / CachyOS

```sh
sudo pacman -S --needed base-devel cmake extra-cmake-modules qt6-base ki18n krunner kconfig
```

### Fedora

```sh
sudo dnf install cmake extra-cmake-modules gcc-c++ make qt6-qtbase-devel kf6-ki18n-devel kf6-krunner-devel kf6-kconfig-devel
```

### Ubuntu / Debian

KF6 development packages are available in recent Ubuntu/Debian releases. Package names may vary slightly by release, but start with:

```sh
sudo apt update
sudo apt install build-essential cmake extra-cmake-modules qt6-base-dev libkf6i18n-dev libkf6runner-dev libkf6config-dev
```

If `libkf6runner-dev` is unavailable, your distribution release may not ship KF6 KRunner development headers yet; use a newer release or install the KDE Frameworks 6 development packages from your distribution's KDE/Plasma repository.

Then build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Install the KRunner plugin

### User-local install

This installs the plugin under `~/.local` without requiring root:

```sh
cmake --install build --prefix ~/.local
```

Restart KRunner so it discovers the new plugin:

```sh
kquitapp6 krunner || true
krunner --daemon
```

Open KRunner and type `bigclock`. If needed, confirm the plugin is enabled in **System Settings → Search → KRunner → Plugins**.

You can also verify that KRunner sees the plugin with:

```sh
krunner --list | grep -i "big clock"
```

Avoid using plain `krunner &` for restart instructions; on some Plasma systems it may print a portal app-ID warning such as `App info not found for 'org.kde.krunner'`. Starting with `--daemon` avoids that foreground-launch warning.

### System-wide install

To install for all users, install into the KDE/Qt prefix, commonly `/usr` on Linux distributions:

```sh
sudo cmake --install build --prefix /usr
kquitapp6 krunner || true
krunner --daemon
```

### Uninstall

Remove the installed module and restart KRunner. For a user-local install:

```sh
rm -f ~/.local/lib/qt6/plugins/kf6/krunner/krunner_bigclock.so
kquitapp6 krunner || true
krunner --daemon
```

Depending on the distribution, the plugin directory may be `~/.local/lib/plugins/kf6/krunner`, `~/.local/lib64/qt6/plugins/kf6/krunner`, or another Qt plugin path; check the `cmake --install` output if the path differs.

## Linting tools

Install the project linting tools with one of:

```sh
# Arch Linux / CachyOS
sudo pacman -S --needed clang clazy cppcheck

# Fedora
sudo dnf install clang-tools-extra clazy cppcheck

# Ubuntu / Debian
sudo apt install clang-format clang-tidy clazy cppcheck
```

Run the checks listed in `AGENTS.md` after code changes.
