#pragma once


#include <boost/property_tree/ptree.hpp>

#include <vector>

#include "product.hpp"
#include "trade.hpp"
#include "ticker.hpp"
#include "stats.hpp"
#include "book.hpp"
#include "bid.hpp"

#include <CryptoAPI/Coinbase/Auth.hpp>


namespace pt = boost::property_tree;

namespace CryptoAPI {

    namespace Coinbase {

    namespace marketdata {

        namespace products {

            std::vector<responses::product> getProducts(const Auth& auth)
            {
                auto resp = auth.GetHttpClient()->LegacyHttpRequest("/products");

                std::vector<responses::product> products;
                for (const auto& prod : resp) {
                    responses::product product(prod.second);
                    products.push_back(product);
                }

                return products;
            }

            responses::product getProduct(const Auth& auth, const std::string& productId)
            {
                auto resp = auth.GetHttpClient()->LegacyHttpRequest("/products/" + productId);

                responses::product product(resp);
                return product;
            }

            responses::ticker getTicker(const Auth& auth, const std::string& productId) {

                std::string target = "/products/" + productId + "/ticker";
                auto resp = auth.GetHttpClient()->LegacyHttpRequest(target);

                responses::ticker ticker(resp);
                return ticker;
            }

            responses::stats getStats(const Auth& auth, const std::string& productId) {
                const auto& httpClient = auth.GetHttpClient();

                std::string target = "/products/";
                target += productId;
                target += "/stats";
                auto resp = httpClient->LegacyHttpRequest(target);

                responses::stats stats(resp);
                return stats;
            }

            std::vector<responses::trade> getTrades(const Auth& auth, const std::string& productId) {
                const auto& httpClient = auth.GetHttpClient();
                std::string target = "/products/";
                target += productId;
                target += "/trades";
                auto resp = httpClient->LegacyHttpRequest(target);

                std::vector<responses::trade> trades;
                for (const auto& tr : resp) {
                    responses::trade currTrade(tr.second);
                    trades.push_back(currTrade);
                }

                return trades;
            }

            template<class X, class Y>
            responses::book<X, Y> getOrderBook(const Auth& auth, const std::string& productId, const std::string& level)
            {
                const auto& httpClient = auth.GetHttpClient();
                std::string target = "/products/";
                target += productId;
                target += "/book";
                target += level;

                auto resp = httpClient->LegacyHttpRequest(target);

                spdlog::debug("After MakeRequest {}", target);

                std::vector<X> bids;
                std::vector<Y> asks;


                spdlog::debug("BEFORE data");
                spdlog::debug("DATA: {}", resp.data());
                spdlog::debug("AFTER data");

                spdlog::debug("before sequence");
                long sequence = resp.get<long>("sequence");
                spdlog::debug("after sequence");

                spdlog::debug("Orderbook Sequence: {}", sequence);

                // property tree limitations
                for (auto& bid : resp.get_child("bids")) {
                    auto bidIter = bid.second.begin();
                    auto price = bidIter->second.data();
                    bidIter++;
                    auto size = bidIter->second.data();
                    bidIter++;
                    auto thirdObj = bidIter->second.data();
                    X currBid(price, size, thirdObj);
                    bids.push_back(currBid);
                }

                for (auto& ask : resp.get_child("asks")) {
                    auto askIter = ask.second.begin();
                    auto price = askIter->second.data();
                    askIter++;
                    auto size = askIter->second.data();
                    askIter++;
                    auto thirdObj = askIter->second.data();
                    Y currAsk(price, size, thirdObj);
                    asks.push_back(currAsk);
                }

                responses::book<X, Y> currBook(bids, asks, sequence);
                return currBook;
            }

            responses::book<responses::bidLevel1_2, responses::askLevel1_2>
                getOrderBookLevelOne(const Auth& auth, const std::string& productId) {
                std::string level = "";
                return getOrderBook<responses::bidLevel1_2, responses::askLevel1_2>(auth, productId, level);
            }

            responses::book<responses::bidLevel1_2, responses::askLevel1_2>
                getOrderBookLevelTwo(const Auth& auth, const std::string& productId) {
                std::string level = "?level=2";
                return getOrderBook<responses::bidLevel1_2, responses::askLevel1_2>(auth, productId, level);
            }

            responses::book<responses::bidLevel3, responses::askLevel3>
                getOrderBookLevelThree(const Auth& auth, const std::string& productId) {
                std::string level = "?level=3";
                return getOrderBook<responses::bidLevel3, responses::askLevel3>(auth, productId, level);
            }

        }

        }
    }
}