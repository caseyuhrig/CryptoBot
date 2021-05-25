#pragma once

#include <CryptoAPI/Base64.hpp>


// https://stackoverflow.com/questions/62359940/how-to-listen-to-websocket-feed-using-boost-beast-websocket-client


namespace CryptoAPI {

    namespace Coinbase {
        namespace beast = boost::beast;
        namespace http = beast::http;
        namespace net = boost::asio;
        using tcp = net::ip::tcp;
        namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
        namespace pt = boost::property_tree;


        class HttpClient
        {
        public:
            enum class RequestVerb { GET, POST, DEL };

            HttpClient(const std::string& apiKey, const std::string& apiSecret, const std::string& passphrase, const bool mode)
                : apiKey(apiKey), apiSecret(apiSecret), passphrase(passphrase), mode(mode)
            {
                version = 11;

                ctx = std::make_shared<ssl::context>(ssl::context::sslv23);
                ioc = std::make_shared<net::io_context>();
                ctx->set_default_verify_paths();

                resolver = std::make_shared<tcp::resolver>((*ioc));
                auto const host = mode ? "api.pro.coinbase.com" : "api-public.sandbox.pro.coinbase.com";
                auto const port = "443";

                results = resolver->resolve(host, port);
            }

            ~HttpClient() = default;

            const std::string GetLocalEpoch() const noexcept
            {
                const auto time = std::time(nullptr);
                const auto epoch = fmt::format("{}", time);
                spdlog::debug("Local EPOCH: [{}]", epoch);
                return epoch;
            }

            const std::string GetRemoteEpoch() const
            {
                // TODO Figure out why GET(...) doesn't work.  content type?
                const auto response = SimpleHttpGet("/time");

                const auto json = nlohmann::json::parse(response);
                if (json.contains("message"))
                {
                    const auto message = json["message"].get<std::string>();
                    const auto exceptionMessage = fmt::format("GetEpoch: {}", message);
                    throw std::runtime_error(exceptionMessage);
                }
                if (json.contains("epoch"))
                {
                    const auto time = json["epoch"].get<double>();
                    const auto epoch = number_to_string<double>(time);
                    spdlog::debug("Remote EPOCH: [{}]", epoch);
                    return epoch;
                }
                throw std::runtime_error("Problem getting remote server epoch.");
            }

            const std::string HttpRequest(
                const std::string& target,
                const std::string& body,
                const HttpClient::RequestVerb rv) const
            {
                http::verb boostVerb;
                std::string messageVerb;
                if (rv == HttpClient::RequestVerb::GET) {
                    messageVerb = "GET";
                    boostVerb = http::verb::get;
                }
                else if (rv == HttpClient::RequestVerb::POST) {
                    messageVerb = "POST";
                    boostVerb = http::verb::post;
                }
                else {
                    messageVerb = "DELETE";
                    boostVerb = http::verb::delete_;
                }

                // construct necessary signature
                const auto timestamp = GetLocalEpoch();
                const auto signature = EncodeSignature(timestamp, messageVerb, target, body);

                auto stream = beast::ssl_stream<beast::tcp_stream>(*ioc, *ctx);

                const auto host = mode ? "api.pro.coinbase.com" : "api-public.sandbox.pro.coinbase.com";

                if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
                    spdlog::error("Error: SNI host name set incorrectly");
                }

                beast::get_lowest_layer(stream).connect(results);

                stream.handshake(ssl::stream_base::client);

                //http::request<http::string_body> req{ boostVerb, target, version };
                auto req = http::request<http::string_body>(boostVerb, target, version);
                req.set(http::field::host, host);
                req.set(http::field::content_type, "application/json");
                req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                req.set("CB-ACCESS-SIGN", signature);
                req.set("CB-ACCESS-TIMESTAMP", timestamp);
                req.set("CB-ACCESS-KEY", apiKey);
                req.set("CB-ACCESS-PASSPHRASE", passphrase);

                if (body.length() != 0)
                {
                    req.body() = body;
                    req.prepare_payload();
                }

                http::write(stream, req);

                beast::flat_buffer buffer;

                http::response<http::dynamic_body> res;

                http::read(stream, buffer, res);

                if (res.result_int() == 400 || res.result_int() == 404)
                {
                    const auto message = fmt::format("Failed {} Response: {}", target, res.result_int());
                    spdlog::critical(message);
                    beast::error_code ec;
                    stream.shutdown(ec);
                    throw std::runtime_error(message);
                }

                const std::string buf = boost::beast::buffers_to_string(res.body().data());
                //spdlog::debug("MakeRequest('{}'): {}", target, buf);
                //spdlog::debug("MakeRequest to '{}', Response: {}", target, buf);
                const auto json = nlohmann::json::parse(buf);

                if (json.contains("message"))
                {
                    const auto message = fmt::format("Failed Response: {}", json["message"].get<std::string>());
                    spdlog::critical(message);
                    beast::error_code ec;
                    stream.shutdown(ec);
                    throw std::runtime_error(message);
                }

                beast::error_code ec;
                stream.shutdown(ec);

                return buf;
            }

            const std::string HttpGet(const std::string& target, const std::string& body)
            {
                return HttpRequest(target, body, HttpClient::RequestVerb::GET);
            }

            const std::string HttpGet(const std::string& target)
            {
                return HttpGet(target, "");
            }

            const std::string HttpPost(const std::string& target, const std::string& body)
            {
                return HttpRequest(target, body, HttpClient::RequestVerb::POST);
            }

            //const std::optional<std::string> GetRequest(const std::string& target) const
            const std::string SimpleHttpGet(const std::string& target) const
            {
                //auto result = std::optional<std::string>();
                //std::optional<std::string> result;

                //try {

                auto stream = beast::ssl_stream<beast::tcp_stream>(*ioc, *ctx);

                const auto host = mode ? "api.pro.coinbase.com" : "api-public.sandbox.pro.coinbase.com";

                if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
                    spdlog::error("Error: SNI host name set incorrectly");
                }

                beast::get_lowest_layer(stream).connect(results);

                stream.handshake(ssl::stream_base::client);

                auto req = http::request<http::string_body>(http::verb::get, target, version);
                req.set(http::field::host, host);
                req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

                http::write(stream, req);

                beast::flat_buffer buffer;
                http::response<http::dynamic_body> res;

                http::read(stream, buffer, res);

                const std::string buf = boost::beast::buffers_to_string(res.body().data());
                //spdlog::debug("GetRequest('{}'): {}", target, buf);

                beast::error_code ec;
                stream.shutdown(ec);

                return buf;
                //}
                //catch (std::exception const& e) {
                //    spdlog::critical("Error: {}", e.what());
                //}
                //return "";
            }

            /**
            * Need to remove ptree and use the n
            */
            pt::ptree LegacyHttpRequest(const std::string& target)
            {
                try {

                    beast::ssl_stream<beast::tcp_stream> stream(*ioc, *ctx);

                    auto const host = mode ? "api.pro.coinbase.com" : "api-public.sandbox.pro.coinbase.com";

                    if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
                        spdlog::error("Error: SNI host name set incorrectly");
                    }

                    beast::get_lowest_layer(stream).connect(results);

                    stream.handshake(ssl::stream_base::client);

                    http::request<http::string_body> req{ http::verb::get, target, version };
                    req.set(http::field::host, host);
                    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

                    http::write(stream, req);

                    beast::flat_buffer buffer;

                    http::response<http::dynamic_body> res;

                    http::read(stream, buffer, res);

                    //std::string buf = boost::beast::buffers_to_string(res.body().data());
                    //spdlog::debug("MakeRequest({}) BUFFER: {}", target, buf);

                    std::stringstream ss;
                    ss << std::string(boost::asio::buffers_begin(res.body().data()),
                        boost::asio::buffers_end(res.body().data()));

                    pt::ptree resp;
                    pt::read_json(ss, resp);

                    beast::error_code ec;
                    stream.shutdown(ec);

                    return resp;
                }
                catch (std::exception const& e) {
                    spdlog::error("Error: {}", e.what());
                    return {};
                }
            }
        private:
            /*
            // OLD CODE FOR encoding the signature, saving because I think this needs some work on
            // how the old vs new std::string's and char* work.  Not sure I'm passing/referencing things properly.
            // Want this code to be 100% solid.
            // Why is the old code using _strdup?  Guessing it's because std::string allocates on the stack?

            const auto message = timestamp + messageVerb + target + body;

            const auto postDecodeSecret = Base64::Decode(apiSecret);

            char* key = _strdup(postDecodeSecret.c_str());
            auto key_len = static_cast<std::int32_t>(strlen(key));

            const auto* data = reinterpret_cast<const unsigned char*>(_strdup(message.c_str()));
            const auto data_len = strlen((char*)data);
            unsigned char* md = 0; //nullptr;
            unsigned int md_len = -1;

            md = HMAC(EVP_sha256(), (const void*)key, key_len, data, data_len, md, &md_len);

            const char* preEncodeSignature_c = _strdup(reinterpret_cast<const char*>(md));
            auto preEncodeSignature = std::string(preEncodeSignature_c);
            auto postEncodeSignature = Base64::Encode(preEncodeSignature);

            free(key);
            free((char*)data);
            free((char*)preEncodeSignature_c);

            boost::replace_all(postEncodeSignature, "\n", "");
            */

            const std::string EncodeSignature(
                const std::string& timestamp,
                const std::string& messageVerb,
                const std::string& target,
                const std::string& body) const
            {
                const auto message = timestamp + messageVerb + target + body;

                const auto key = Base64::Decode(apiSecret);

                unsigned char* md = nullptr;
                unsigned int md_len = -1;

                md = HMAC(EVP_sha256(), 
                    reinterpret_cast<const void*>(key.c_str()), key.length(),
                    reinterpret_cast<const unsigned char*>(message.c_str()), message.length(), 
                    md, &md_len);

                auto signature = Base64::Encode(std::string(reinterpret_cast<char*>(md)));
                signature.erase(std::remove(signature.begin(), signature.end(), '\n'), signature.end());
                return signature;
            }

            template <typename T>
            const std::string number_to_string(const T& t) const
            {
                static_assert(std::is_arithmetic<T>::value, "number_to_string<T>(const T value) must be numeric");
                std::string str = std::to_string(t);
                int offset = 1;
                if (str.find_last_not_of('0') == str.find('.')) {
                    offset = 0;
                }
                str.erase(str.find_last_not_of('0') + offset, std::string::npos);
                return str;
            }
        private:
            std::shared_ptr<net::io_context> ioc;
            std::shared_ptr<ssl::context> ctx;
            std::shared_ptr<tcp::resolver> resolver;
            boost::asio::ip::tcp::resolver::results_type results;

            std::string apiKey;
            std::string apiSecret;
            std::string passphrase;

            bool mode;
            int version;
        };

        /*
        class MessagingException : public std::exception
        {
        public:
            MessagingException(const std::string& message) : m_Message(message) {}
            MessagingException() = default;
        private:
            std::string m_Message;
            std::exception m_Cause;
        };
        */
    }
}