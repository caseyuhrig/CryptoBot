#pragma once

#include <string>

namespace CryptoAPI {
    namespace responses {
        class bidLevel1_2 {
        public:
            bidLevel1_2(std::string price, std::string size, std::string numOrders) : price(std::move(price)),
                size(std::move(size)),
                numOrders(std::move(
                    numOrders)) {}

            const std::string& getPrice() const {
                return price;
            }

            const std::string& getSize() const {
                return size;
            }

            const std::string& getNumOrders() const {
                return numOrders;
            }
        private:
            std::string price;
            std::string size;
            std::string numOrders;

        };

        class bidLevel3 {
        public:
            bidLevel3(std::string price, std::string size, std::string orderId) : price(std::move(price)),
                size(std::move(size)),
                orderId(std::move(orderId)) {}

            const std::string& getPrice() const {
                return price;
            }

            const std::string& getSize() const {
                return size;
            }

            const std::string& getOrderId() const {
                return orderId;
            }
        private:
            std::string price;
            std::string size;
            std::string orderId;
        };
    } // namespace responses
}