// @file      hot_reload.h
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved
//
// Generic Hot Reload System using Class Factory Pattern

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <dlfcn.h>
#include <sys/stat.h>
#include <cstdio>

namespace HotReload {

// Base interface for reloadable objects
class IReloadable {
public:
    virtual ~IReloadable() = default;
    virtual int GetVersion() const = 0;
};

// Factory function type
typedef void* (*CreateFactoryFunc)();
typedef void (*DestroyFactoryFunc)(void*);

// Hot reload manager template class
template<typename T>
class Manager {
private:
    void* libraryHandle;
    CreateFactoryFunc createFunc;
    DestroyFactoryFunc destroyFunc;
    std::string libraryPath;
    time_t lastModTime;
    int currentVersion;
    std::unique_ptr<T, std::function<void(T*)>> instance;

    // Get file modification time
    time_t GetFileModTime(const char* path) {
        struct stat fileStat;
        if (stat(path, &fileStat) == 0) {
            return fileStat.st_mtime;
        }
        return 0;
    }

    // Load or reload library
    bool LoadLibrary() {
        // Unload previous library if exists
        if (libraryHandle != nullptr) {
            instance.reset();  // This will now call the custom deleter
            dlclose(libraryHandle);
            libraryHandle = nullptr;
            printf("Unloaded previous library\n");
        }

        // Load library
        printf("Loading library from: %s\n", libraryPath.c_str());
        libraryHandle = dlopen(libraryPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!libraryHandle) {
            fprintf(stderr, "Failed to load library: %s\n", dlerror());
            return false;
        }
        printf("Library loaded successfully\n");

        // Load function pointers
        createFunc = (CreateFactoryFunc)dlsym(libraryHandle, "CreateInstance");
        destroyFunc = (DestroyFactoryFunc)dlsym(libraryHandle, "DestroyInstance");

        if (!createFunc) {
            fprintf(stderr, "Failed to load CreateInstance function: %s\n", dlerror());
            dlclose(libraryHandle);
            libraryHandle = nullptr;
            return false;
        }

        if (!destroyFunc) {
            fprintf(stderr, "Failed to load DestroyInstance function: %s\n", dlerror());
            dlclose(libraryHandle);
            libraryHandle = nullptr;
            return false;
        }

        // Create instance - cast void* to T*
        void* rawInstance = createFunc();
        if (!rawInstance) {
            fprintf(stderr, "Failed to create instance\n");
            dlclose(libraryHandle);
            libraryHandle = nullptr;
            return false;
        }

        // Cast and wrap in unique_ptr with custom deleter
        T* typedInstance = static_cast<T*>(rawInstance);

        // Create custom deleter that uses the library's DestroyInstance function
        auto customDeleter = [this](T* ptr) {
            if (ptr && destroyFunc) {
                destroyFunc(static_cast<void*>(ptr));
            }
        };

        instance = std::unique_ptr<T, std::function<void(T*)>>(typedInstance, customDeleter);

        // Update version and modification time
        if constexpr (std::is_base_of_v<IReloadable, T>) {
            currentVersion = instance->GetVersion();
            printf("Loaded library version %d\n", currentVersion);
        }

        lastModTime = GetFileModTime(libraryPath.c_str());
        return true;
    }

public:
    Manager() : libraryHandle(nullptr), createFunc(nullptr), destroyFunc(nullptr),
                lastModTime(0), currentVersion(0) {}

    ~Manager() {
        if (libraryHandle) {
            instance.reset();
            dlclose(libraryHandle);
        }
    }

    // Initialize with library path
    bool Initialize(const std::string& libPath) {
        libraryPath = libPath;
        return LoadLibrary();
    }

    // Check if library needs reload and reload if necessary
    bool CheckAndReload() {
        time_t currentModTime = GetFileModTime(libraryPath.c_str());

        if (currentModTime > lastModTime) {
            printf("Library file changed, reloading...\n");
            return LoadLibrary();
        }

        return false;
    }

    // Get current instance
    T* GetInstance() {
        return instance.get();
    }

    // Check if instance is valid
    bool IsValid() const {
        return instance != nullptr;
    }

    // Get current version
    int GetVersion() const {
        return currentVersion;
    }
};

} // namespace HotReload

