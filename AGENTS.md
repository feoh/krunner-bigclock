# AGENTS.md

## Project
KRunner Bigclock is a KDE Frameworks 6 / Qt 6 C++ KRunner plugin. Keep the plugin small, responsive, and safe to load in the KRunner process.

## KDE / Qt development practices
- Use modern C++20 with Qt parent ownership; avoid raw owning pointers unless QObject parenting or RAII makes ownership explicit.
- Keep runner `match()` fast and non-blocking. Do heavy work in `init()` or worker objects only when necessary.
- Only create or manipulate widgets from `run()` or the GUI thread.
- Use KDE metadata (`K_PLUGIN_CLASS_WITH_JSON`) and install runner modules to `${KDE_INSTALL_PLUGINDIR}/kf6/krunner`.
- Use `QStringLiteral`, `QLatin1StringView`, and `i18n()` for user-visible strings.
- Prefer KDE/Qt APIs over platform-specific code. Respect Wayland and multi-screen behavior.
- Add SPDX copyright/license headers to source files.
- Keep CMake target-based: `target_sources`, `target_link_libraries`, no global include/link flags unless unavoidable.
- Do not block, sleep, perform network I/O, or spawn processes in `match()`.
- Test with KRunner after install: restart KRunner with `kquitapp6 krunner; krunner` when needed.

## Formatting and linters to run after every code change
Run these from the repository root after each C++/Qt/CMake change:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
clang-format -i $(find src -name '*.cpp' -o -name '*.h')
cmake -S . -B build-clang -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=clang++
cmake --build build-clang
clang-tidy -p build-clang $(find src -name '*.cpp')
clazy-standalone -p build-clang $(find src -name '*.cpp')
cppcheck --enable=warning,style,performance,portability --std=c++20 --project=build/compile_commands.json --file-filter="$(pwd)/src/*" --suppress=preprocessorErrorDirective
```

If a tool is unavailable in the local environment, note that explicitly and still run the remaining checks. Re-run `cmake --build build` after any formatter/linter autofix.
