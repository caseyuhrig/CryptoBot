#pragma once

#include <CryptoAPI/PostgreSQL.hpp>
#include <nlohmann/json.hpp>
#include <string>


namespace CryptoAPI {

    namespace Coinbase {

        class TradingAccount
        {
        public:
            TradingAccount(const nlohmann::json& json)
                : m_ID(0), m_Exchange("COINBASE")
            {
                m_AccountID = json["id"];
                m_ProfileID = json["profile_id"];
                m_Currency = json["currency"];
                m_Balance = json["balance"];
                m_Available = json["available"];
                m_Hold = json["hold"];
                m_TradingEnabled = json["trading_enabled"];
            }

            ~TradingAccount() = default;

            inline const std::uint64_t GetID() const { return m_ID; }
            inline const std::string GetExchange() const { return m_Exchange; }
            inline const std::string GetAccountID() const { return m_AccountID; }
            inline const std::string getProfileID() const { return m_ProfileID; }
            inline const std::string GetCurrency() const { return m_Currency; }
            inline const std::string GetBalance() const { return m_Balance; }
            inline const std::string GetAvailable() const { return m_Available; }
            inline const std::string GetHold() const { return m_Hold; }
            inline const bool GetTradingEnabled() const { return m_TradingEnabled; }

            void Save()
            {
                if (m_ID == 0) {
                    m_ID = DB->InsertAccount(m_Exchange, m_AccountID, m_ProfileID, m_Currency, m_Balance, m_Available, m_Hold, m_TradingEnabled);
                }
                else {
                    spdlog::critical("Implement TradingAccount.Save/Update");
                }
            }
        private:
            std::uint64_t m_ID;
            std::string m_Exchange;
            std::string m_AccountID;
            std::string m_ProfileID;
            std::string m_Currency;
            std::string m_Balance;
            std::string m_Available;
            std::string m_Hold;
            bool m_TradingEnabled;
        };
    }
}