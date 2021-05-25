#pragma once

#include <stdafx.hpp>


namespace cu {

	template<typename T>
	using ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr ref<T> create(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}


	template<typename C, typename F>
	auto vmap(F f, const C& c) -> std::vector<decltype(f(*c.begin()))> {
		std::vector<decltype(f(*c.begin()))> res;
		for (auto& x : c) res.push_back(f(x));
		return res;
	}

	//std::vector<T> query(const pqxx::connection& connection, const std::string_view& sql, std::function<const pqxx::connection&> func)

	template <typename T>
	std::optional<T> query_value(pqxx::connection& connection, const std::string& sql)
	{
		try {
			spdlog::debug("QUERY: {}", sql);
			auto tx = pqxx::work(connection);
			const T result = tx.query_value<T>(sql);
			tx.commit();
			return result;
		}
		catch (const pqxx::sql_error exception) {
			spdlog::error("SQL_EX:{} {} {}", exception.sqlstate(), exception.what(), exception.query());
		}
		catch (const std::exception& exception) {
			spdlog::error("EX: {}", exception.what());
		}
	}

	template <typename T>
	std::vector<T> query(pqxx::connection& connection, const std::string& sql, std::function<T(const pqxx::row&)> func)
	{
		spdlog::debug("QUERY: {}", sql);
		auto results = std::vector<T>();
		try {
			auto tx = pqxx::work(connection);
			const auto rows = tx.exec(sql);
			for (const auto row : rows)
			{
				results.push_back(func(row));
			}
			tx.commit();
		}
		catch (const pqxx::sql_error exception) {
			spdlog::error("SQL_EX:{} {} {}", exception.sqlstate(), exception.what(), exception.query());
		}
		catch (const std::exception& exception) {
			spdlog::error("EX: {}", exception.what());
		}
		return results;
	}


	// trim from start (in place)
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}

	namespace ex {

		class file_not_found : public std::exception
		{
		public:
			file_not_found(const std::filesystem::path& path)
				: std::exception(fmt::format("Path {} not found.", path.string()).c_str()),
				m_Path(path) {}

			const std::filesystem::path path() const { return m_Path; }
		private:
			std::filesystem::path m_Path;
		};
	}

	class user
	{
	public:
		static std::filesystem::path home_path()
		{
			spdlog::debug("USERPROFILE = {}", getenv("USERPROFILE"));
			spdlog::debug("HOMEDRIVE   = {}", getenv("HOMEDRIVE"));
			spdlog::debug("HOMEPATH    = {}", getenv("HOMEPATH"));
			const auto folder = getenv("USERPROFILE");
			const auto path = std::filesystem::path(folder);
			return path;
		}
	};

	class properties
	{
	public:
		static std::map<std::string, std::string> load(const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path))
				throw cu::ex::file_not_found(path);

			//auto expr = std::regex("^$", std::regex_constants::icase);
			std::map<std::string, std::string> properties;
			std::fstream file;
			file.open(path.string().c_str(), std::ios::in);
			if (file.is_open())
			{
				std::string line;
				while (std::getline(file, line))
				{
					const auto index = line.find_first_of('=');
					if (index != std::string::npos)
					{
						const auto key = trim_copy(line.substr(0, index));
						const auto value = trim_copy(line.substr(index + 1));
						//spdlog::debug("[{}] = [{}]", key, value);
						properties[key] = value;
					}
				}
				file.close();
			}
			return properties;
		}
	};


}
