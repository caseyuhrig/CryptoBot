#pragma once


#include <CryptoAPI/Coinbase/TradingAccount.hpp>
#include <CryptoAPI/Coinbase/Auth.hpp>
#include <CryptoAPI/PostgreSQL.hpp>
#include <CryptoAPI/Coin/TradingPair.hpp>
#include <CryptoAPI/Coinbase/Order.hpp>


// https://api.pro.coinbase.com
// Sandbox URLs
// When testing your API connectivity, make sure to use the following URLs.

//    Website
//    https://public.sandbox.pro.coinbase.com

// REST API
//    https://api-public.sandbox.pro.coinbase.com

// Websocket Feed
//    wss://ws-feed-public.sandbox.pro.coinbase.com

// FIX API
//    tcp + ssl ://fix-public.sandbox.pro.coinbase.com:4198

// https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
// https://gist.github.com/barrysteyn/7308212


namespace CryptoAPI {

    namespace Coinbase {

        class CoinbaseEx
        {
        public:
            CoinbaseEx(const Auth& auth) : m_Auth(auth) {}

            ~CoinbaseEx() = default;

            void PlaceOrder(const std::string& tradingPair, const std::string& size, const std::string& price) const
            {
                try {
                    const std::string body =
                        "{"
                        "\"type\": \"limit\","
                        "\"product_id\": \"" + tradingPair + "\","
                        "\"size\": \"" + size + "\","
                        "\"price\": \"" + price + "\","
                        "\"side\": \"buy\""
                        "}";
                    spdlog::debug("Order.Request: {}", body);
                    const auto response = m_Auth.GetHttpClient()->HttpPost("/orders", body);
                    spdlog::debug("Order.Response: {}", response);
                    
                    auto order = Order(response);
                    order.Save();
                }
                catch (const std::exception& ex) {
                    spdlog::error("CoinbaseEx.PlaceOrder: {}", ex.what());
                }
            }

            const std::vector<Order> SyncOrders() const
            {
                auto orders = std::vector<Order>();
                try {
                    const auto response = m_Auth.GetHttpClient()->HttpGet("/orders?status=all");
                    const auto json = nlohmann::json::parse(response);
                    for (const auto& element : json) {
                        auto order = Order(element);
                        order.Save();
                        orders.push_back(order);
                    }
                }
                catch (const std::exception& ex) {
                    spdlog::error("CoinbaseEx.GetOrders: {}", ex.what());
                }
                return orders;
            }

            const std::vector<TradingAccount> GetTradingAccounts() const
            {
                auto accounts = std::vector<TradingAccount>();
                try {
                    //const auto jsonText = m_Auth.GetHttpClient()->HttpRequest(
                    //                                "/accounts", "", HttpClient::RequestVerb::GET);
                    const auto response = m_Auth.GetHttpClient()->HttpGet("/accounts");
                    const auto json = nlohmann::json::parse(response);
                    for (const auto& elem : json) {
                        const auto account = TradingAccount(elem);
                        accounts.push_back(account);
                    }                   
                }
                catch (const std::exception& ex) {
                    spdlog::error("GetTradingAccounts: {}", ex.what());
                }
                return accounts;
            }

            const std::vector<Coin::TradingPair> GetCurrencyPairs() const
            {
                auto pairs = std::vector<Coin::TradingPair>();
                const auto response = m_Auth.GetHttpClient()->HttpGet("/products");
                const auto json = nlohmann::json::parse(response);
                for (const auto& element : json)
                {
                    const auto& identifier = element["id"];
                    const auto& base_currency = element["base_currency"];
                    const auto& quote_currency = element["quote_currency"];
                    const auto pair = Coin::TradingPair(identifier, base_currency, quote_currency);
                    pairs.push_back(pair);
                }
                return pairs;
            }

            const std::vector<std::string> GetCurrencies() const
            {
                auto currencies = std::vector<std::string>();
                const auto text = m_Auth.GetHttpClient()->HttpGet("/currencies");
                const auto json = nlohmann::json::parse(text);
                spdlog::debug("JSON (/currencies): {}", json.dump(2, ' '));
                for (const auto& element : json) 
                {
                    const auto& id = element["id"];
                    currencies.push_back(id);
                }
                return currencies;
            }
        private:
            Auth m_Auth;
        };
    }
}