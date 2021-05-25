#pragma once

#include <vector>
#include <string>

#include "bid.hpp"
#include "ask.hpp"

namespace CryptoAPI {
    namespace responses {
        template <class X, class Y>
        class book {
        public:
            book(std::vector<X> bids, std::vector<Y> asks, long sequence) : bids(std::move(bids)),
                asks(std::move(asks)),
                sequence(sequence) {}
            const std::vector<X>& getBids() const {
                return bids;
            }

            const std::vector<Y>& getAsks() const {
                return asks;
            }

            long getSequence() const {
                return sequence;
            }

        private:
            std::vector<X> bids;
            std::vector<Y> asks;
            long sequence;
        };
    } // namespace responses
}