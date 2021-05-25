#pragma once

#include <boost/property_tree/ptree.hpp>


namespace CryptoAPI {

    namespace pt = boost::property_tree;

    namespace responses {

        class ticker
        {
        public:
            ticker(const pt::ptree& data) : data(data) {}

            std::string
                getSize() const {
                return data.get<std::string>("size");
            }

            std::string
                getTime() const {
                return data.get<std::string>("time");
            }

            std::string
                getAsk() const {
                return data.get<std::string>("ask");
            }

            std::string
                getBid() const {
                return data.get<std::string>("bid");
            }

            std::string
                getVolume() const {
                return data.get<std::string>("volume");
            }

            std::string
                getPrice() const {
                return data.get<std::string>("price");
            }

            std::string
                getTradeId() const {
                return data.get<std::string>("trade_id");
            }
        private:
            pt::ptree data;
        };
    } // namespace responses
}