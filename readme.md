# exqudens-cpp-threadpool

## how-to-test-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-test || exit 255"`

## how-to-build-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-install || exit 255"`

## how-to-export-all-presets

1. `conan list 'exqudens-cpp-threadpool/*'`
2. `conan remove -c 'exqudens-cpp-threadpool'`
3. `git clean -xdf`
4. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
5. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target conan-export || exit 255"`

## vscode

1. `git clean -xdf`
2. `cmake --preset ${preset}`
3. `cmake --build --preset ${preset} --target vscode`
4. use vscode debugger launch configurations: `test-app`

### extensions

For `command-variable-launch.json`
use [Command Variable](https://marketplace.visualstudio.com/items?itemName=rioj7.command-variable#pickstringremember) `version >= v1.69.0`

## windows

- set value `REALTIME_PRIORITY_CLASS` using [SetPriorityClass](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass)
- get current using [GetCurrentProcess](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess)
- for threads begin [timeBeginPeriod](https://learn.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timebeginperiod)
- for threads end [timeEndPeriod](https://learn.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timeendperiod)
