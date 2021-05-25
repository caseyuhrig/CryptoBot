#pragma once

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace CryptoAPI {
    namespace responses {

        class trade
        {
        public:
            explicit trade(const pt::ptree& data) : data(data) {}

            std::string getPrice() const {
                return data.get<std::string>("price");
            }

            std::string getSize() const {
                return data.get<std::string>("size");
            }

            std::string getSide() const {
                return data.get<std::string>("side");
            }

            std::string getTime() const {
                return data.get<std::string>("time");
            }

            std::string getTradeId() const {
                return data.get<std::string>("trade_id");
            }
        private:
            pt::ptree data;
        };
    } // namespace responses
}