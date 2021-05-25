#pragma once

#include <string>
#include <CryptoAPI/PostgreSQL.hpp>


namespace CryptoAPI {

    namespace Coin {

        class TradingPair
        {
        public:
            TradingPair(const std::string& identifier, const std::string& baseCurrency, const std::string& quoteCurrency) 
                : m_ID(-1), m_Identifier(identifier), m_BaseCurrency(baseCurrency), m_QuoteCurrency(quoteCurrency)
            {
            }

            ~TradingPair() = default;
            
            const std::string GetIdentifier() const { return m_Identifier; }

            const void Save() const
            {
                try {
                    DB->InsertTradingPair(m_Identifier, m_BaseCurrency, m_QuoteCurrency);
                }
                catch (const std::exception& exception) {
                    spdlog::error("TradingPair.Save(): {}", exception.what());
                }
            }
        private:
            std::int64_t m_ID;
            std::string m_Identifier;
            std::string m_BaseCurrency;
            std::string m_QuoteCurrency;
        };
    }
}