#pragma once

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <exception>
#include <filesystem>


using Path = std::filesystem::path;
using Properties = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
using String = std::string;
template <typename T>
using Vector = std::vector<T>;
using Exception = std::exception;

