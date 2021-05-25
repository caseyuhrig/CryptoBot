#pragma once

#include <cu/common.hpp>

#include <CryptoAPI/Coinbase/HttpClient.hpp>


namespace CryptoAPI {

    namespace Coinbase {

        class Auth
        {
        public:
            enum class Mode { LIVE, SANDBOX };

            Auth(const std::string& apiKey, const std::string& apiSecret, const std::string& passphrase, const Mode& mode)
                : apiKey(apiKey), apiSecret(apiSecret), passphrase(passphrase), mode(mode)
            {
                bool authMode = mode == Auth::Mode::LIVE;
                //spdlog::debug("Before Ref<HttpClient>");
                m_HttpClient = cu::create<HttpClient>(apiKey, apiSecret, passphrase, authMode);
                //spdlog::debug("After Ref<HttpClient>");
            }

            ~Auth() = default;

            const std::string& GetApiKey() const {
                return apiKey;
            }

            const std::string& GetApiSecret() const {
                return apiSecret;
            }

            const std::string& GetPassphrase() const {
                return passphrase;
            }

            const Mode GetMode() const {
                return mode;
            }

            const cu::ref<HttpClient>& GetHttpClient() const
            {
                return m_HttpClient;
            }
        private:
            cu::ref<HttpClient> m_HttpClient;
            std::string apiKey;
            std::string apiSecret;
            std::string passphrase;
            Mode mode;
        };
    }
}