#pragma once

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace CryptoAPI {
    namespace responses {
        class product {
        public:
            product(const pt::ptree& data) : data(data) {}

            [[nodiscard]] std::string getId() const {
                return data.get<std::string>("id");
            }

            std::string
                getDisplayName() const {
                return data.get<std::string>("display_name");
            }

            std::string
                getBaseCurrency() const {
                return data.get<std::string>("base_currency");
            }

            std::string
                getQuoteCurrency() const {
                return data.get<std::string>("quote_currency");;
            }

            std::string
                getBaseIncrement() const {
                return data.get<std::string>("base_increment");
            }

            std::string
                getQuoteIncrement() const {
                return data.get<std::string>("quote_increment");
            }

            std::string
                getBaseMinSize() const {
                return data.get<std::string>("base_min_size");
            }

            std::string
                getBaseMaxSize() const {
                return data.get<std::string>("base_max_size");
            }

            std::string
                getMinMarketFunds() const {
                return data.get<std::string>("min_market_funds");
            }

            std::string
                getMaxMarketFunds() const {
                return data.get<std::string>("max_market_funds");
            }

            std::string
                getStatus() const {
                return data.get<std::string>("status");
            }

            std::string
                getStatusMessage() const {
                return data.get<std::string>("status_message");
            }

            bool getCancelOnly() const {
                return data.get<bool>("cancel_only");
            }

            bool getLimitOnly() const {
                return data.get<bool>("limit_only");
            }

            bool getPostOnly() const {
                return data.get<bool>("post_only");
            }

            bool getTradingDisabled() const {
                return data.get<bool>("trading_disabled");
            }

        private:
            pt::ptree data;
        };
    } // namespace responses
}