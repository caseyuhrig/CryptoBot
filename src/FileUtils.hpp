#pragma once

#include "Core.hpp"


namespace FileUtils
{
    static bool FilesDifferent(const Path& source, const Path& dest);


    static Vector<Path> FindFiles(const Path& path, const String& extension)
    {
        Vector<Path> paths;
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
        {
            for (auto& entry : std::filesystem::recursive_directory_iterator(path))
            {
                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == extension)
                {
                    paths.emplace_back(entry.path());
                }
            }
        }
        return paths;
    }

    static Vector<Path> FindFiles(const Path& path, const Vector<String>& extensions)
    {
        Vector<Path> paths;
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
        {
            for (auto& entry : std::filesystem::recursive_directory_iterator(path))
            {
                if (entry.is_regular_file()) // && !entry.is_directory())
                {
                    if (VectorUtils::Contains(extensions, entry.path().extension().string()))
                    {
                        paths.emplace_back(entry.path());
                    }
                    else {
                        spdlog::error("NOT {}", entry.path().string());
                    }
                }
            }
        }
        return paths;
    }

    static void CopyFile(const Path& source, const Path& dest)
    {
        try {
            const Path& parent = dest.parent_path();
            if (!std::filesystem::exists(parent))
            {
                if (!std::filesystem::create_directories(parent))
                {
                    spdlog::critical("ERROR: cannot create: {}", parent.string());
                }
            }
            if (FilesDifferent(source, dest))
            {
                spdlog::info("Copying: {} to {}", source.string(), dest.string());
                std::filesystem::copy_file(source, dest, std::filesystem::copy_options::overwrite_existing);
            }
        }
        catch (const Exception& exception)
        {
            spdlog::critical("Exception: {}", exception.what());
        }
    }

    static void CopyFiles(const Path& source, const Path& dest, const String& extension)
    {
        //spdlog::debug("Copy: {1}\\*{0} to {2}", extension, source.string(), dest.string());
        auto sourcePaths = FindFiles(source, extension);
        for (auto& contentPath : sourcePaths)
        {
            auto outputPath = Path(dest.string() + contentPath.string().substr(source.string().length())).lexically_normal();
            CopyFile(contentPath, outputPath);
        }
    }

    static void CopyFiles(const Path& source, const Path& dest, const Vector<String>& extensions)
    {
        for (auto& extension : extensions)
            CopyFiles(source, dest, extension);
    }

    static uintmax_t FileSize(const Path& path)
    {
        try {
            return std::filesystem::file_size(path);
        }
        catch (const std::exception& exception)
        {
            spdlog::critical("Exception: {}", exception.what());
        }
        return -1;
    }

    static bool FilesDifferent(const Path& source, const Path& dest)
    {
        if (!std::filesystem::exists(dest)) return true;
        return std::filesystem::file_size(source) != std::filesystem::file_size(dest);
    }
}

namespace VFS {

    struct File
    {
        std::string filePath;
        uintmax_t size;

        File(const std::string& fp, const uintmax_t s) : filePath(fp), size(s) {}
    };
}

    
