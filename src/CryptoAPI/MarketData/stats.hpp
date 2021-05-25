#pragma once

#include <boost/property_tree/ptree.hpp>


namespace CryptoAPI {

    namespace pt = boost::property_tree;

    namespace responses {
        class stats {
        public:
            std::string
                getOpen() const {
                return data.get<std::string>("open");
            }

            std::string
                getHigh() const {
                return data.get<std::string>("high");
            }

            std::string
                getLow() const {
                return data.get<std::string>("low");
            }

            std::string
                getVolume() const {
                return data.get<std::string>("volume");
            }

            std::string
                getLast() const {
                return data.get<std::string>("last");
            }

            std::string
                getVolume30Day() const {
                return data.get<std::string>("volume_30day");
            }

            stats(const pt::ptree& data) : data(data) {}
        private:
            pt::ptree data;
        };
    } // namespace responses
}