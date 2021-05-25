#pragma once



#include <plplot/plplot.h>

//#include <pqxx/pqxx>
//#include <libpq-fe.h>
#include <tao/pq.hpp>

#include <cu/common.hpp>

#include <spdlog/spdlog.h>

#include <fmt/format.h>
//#include <matplotlibcpp.h>

#include <string>


// issue linking libpq, add the additional libs to the linker config
// https://github.com/microsoft/vcpkg/issues/16423

namespace CryptoAPI {



    class PostgreSQL
    {
    public:
        PostgreSQL(const std::string& hostname, const std::string& database, 
            const std::string& username, const std::string& password) 
            : m_Hostname(hostname), m_Database(database), m_Username(username), m_Password(password) {}
        ~PostgreSQL() = default;

        static const cu::ref<PostgreSQL> Create(const std::string& hostname, const std::string& database, 
            const std::string& username, const std::string& password)
        {
            return cu::create<PostgreSQL>(hostname, database, username, password);
        }

        void Connect()
        {
            const auto connectionInfo = fmt::format("postgresql://{}:{}@{}/{}", m_Username, m_Password, m_Hostname, m_Database);
            spdlog::info("Connecting: {}", connectionInfo);
            m_Connection = tao::pq::connection::create(connectionInfo);
            /*
            const auto res = m_Connection->execute("SELECT id, identifier FROM exchange");
            for (auto& row : res)
            {
                auto id = row.get<std::uint64_t>(0);
                auto identifier = row.get<std::string>(1);
                spdlog::debug("ROW: {} {}", id, identifier);
            }
            */
            /*
            if (PQstatus(m_Connection) != CONNECTION_OK) 
            {
                auto message = std::string(PQerrorMessage(m_Connection));
                spdlog::critical("Connection to database failed: {}", message);
                PQfinish(m_Connection);
            }
            else {
                spdlog::debug("Connection to database succeed.");
            }
            */
        }

        void CreateGraph() const
        {
            spdlog::debug("Create graph");
            try {
                const auto bounds = GetMinMax();
                //PLFLT_VECTOR v;
                const auto max = GetCount();
                auto x = std::vector<std::double_t>(max);
                auto y = std::vector<std::double_t>(max);
                //auto size = GetCount("ticker");
                auto n = 0;
                spdlog::trace("before query");
                const auto query = fmt::format("SELECT id, price, extract(epoch from create_date) as epoch FROM ticker WHERE trading_pair_id = 1 LIMIT {}", max);
                spdlog::debug("query: {}", query);
                const auto res = m_Connection->execute(query);
                spdlog::trace("before1");
                for (auto& row : res) {
                    x[n] = row.get<std::double_t>(1); // price
                    y[n] = row.get<std::double_t>(2); // epoch
                    n++;
                }
                spdlog::trace("before2");
                plsdev("wingcc");
                //plsetopt();
                plinit();
                //spdlog::trace("before2.1");
                //c_plenv(-1.3, 1.3, -1.3, 1.3, 1, -2);
                plenv(bounds[2], bounds[3], bounds[0], bounds[1], 0, 0);
                //pllab("x", "y=100 x#u2#d", "Simple PLplot demo of a 2D line plot");
                pllab("epoch", "price", "Simple PLplot demo of a 2D line plot");
                spdlog::trace("before3");
                plline(max-1, y.data(), x.data());
                spdlog::trace("before4");
                plend();
                spdlog::trace("before5");
            } catch (const std::exception& exception) {
                spdlog::error("PostgreSQL::CreateGraph: {}", exception.what());
            }
        }

        const std::vector<std::double_t> GetMinMax() const
        {
            auto results = std::vector<std::double_t>(4);
            try {
                auto query = R"(
                    SELECT 
                        min(price) as min_price, 
                        max(price) as max_price, 
                        min(extract(epoch from price_time)) as min_epoch, 
                        max(extract(epoch from create_date)) as max_epoch 
                    FROM
                        ticker
                    WHERE
                        trading_pair_id = 1
                )";
                const auto res = m_Connection->execute(query);
                for (auto& row : res) {
                    results[0] = row.get<std::double_t>(0);
                    results[1] = row.get<std::double_t>(1);
                    results[2] = row.get<std::double_t>(2);
                    results[3] = row.get<std::double_t>(3);
                }
            } catch (const std::exception& exception) {
                spdlog::error("PostgreSQL::CreateGraph: {}", exception.what());
            }
            return results;
        }

        const std::size_t GetCount() const
        {
            std::size_t count = 0;
            try {
                std::size_t n = 0;
                const auto res = m_Connection->execute("select count(*) from ticker WHERE trading_pair_id = 1");
                for (auto& row : res) {
                    return row.get<std::size_t>(0);
                }
            }
            catch (const std::exception& exception)
            {
                spdlog::error("PostgreSQL::GetCount: {}", exception.what());
            }
            return count;
        }

        const std::string GetPairList() const
        {
            std::string results = "";
            try {
                std::size_t n = 0;
                const auto res = m_Connection->execute("select identifier from trading_pair where quote_currency = 'USD'");
                for (auto& row : res)
                {
                    if (n > 0) results += ",";
                    results += "\"" + row.get<std::string>(0) + "\"";
                    n++;
                }
            }
            catch (const std::exception& exception)
            {
                spdlog::error("PostgreSQL::InsertTickerAmount: {}", exception.what());
            }
            return results;
        }

        void InsertTickerAmount(const std::string& tradingPair, const std::uint64_t sequence, const std::string price,
            const std::string& priceTime,
            const std::uint64_t& exchangeTradeID,
            const std::string& lastSize,
            const std::string& side,
            const std::string& open24h,
            const std::string& volume24h,
            const std::string& low24h,
            const std::string& high24h,
            const std::string& volume30d,
            const std::string& bestBid,
            const std::string& bestAsk)
        {
            try {
                const auto transaction = m_Connection->transaction();
                transaction->execute(R"(
                    INSERT INTO ticker (
                        exchange_id, 
                        trading_pair_id, 
                        sequence, 
                        price,
                        price_time,
                        exchange_trade_id,
                        last_size,
                        side,
                        open_24h,
                        volume_24h,
                        low_24h,
                        high_24h,
                        volume_30d,
                        best_bid,
                        best_ask
                    ) VALUES (
                        (SELECT id FROM exchange WHERE identifier = $1), 
                        (SELECT id FROM trading_pair WHERE identifier = $2), 
                        $3, 
                        $4,
                        $5::TIMESTAMP WITH TIME ZONE, 
                        $6, $7, 
                        (SELECT id FROM side WHERE identifier = UPPER($8)), 
                        $9, $10, $11, $12, $13, $14, $15
                    )
                )", "COINBASE", tradingPair, sequence, price, priceTime, exchangeTradeID,
                    lastSize,
                    side,
                    open24h,
                    volume24h,
                    low24h,
                    high24h,
                    volume30d,
                    bestBid,
                    bestAsk);
                transaction->commit();
            }
            catch (const std::exception& exception) 
            {
                spdlog::error("PostgreSQL::InsertTickerAmount: {}", exception.what());
            }
        }

        void InsertTradingPair(const std::string& pair, const std::string& base, const std::string& quote) const
        {
            try {
                const auto transaction = m_Connection->transaction();
                transaction->execute("INSERT INTO trading_pair (identifier, base_currency, quote_currency) VALUES ($1, $2, $3)", pair, base, quote);
                transaction->commit();
            }
            catch (const std::exception& exception)
            {
                spdlog::error("PostgreSQL::InsertTradingPair: {}", exception.what());
            }
        }

        std::uint64_t InsertAccount(std::string exchange, std::string accountID, std::string profileID, 
            std::string currency, std::string balance, std::string available, std::string hold, 
            bool tradingEnabled) const
        {
            try {
                const auto query = R"(
                    INSERT INTO trading_account (
                        exchange,
                        account_id,
                        profile_id,
                        currency,
                        balance,
                        available,
                        hold,
                        trading_enabled
                    ) VALUES (
                        $1, $2, $3, $4, $5, $6, $7, $8
                    )
                )";
                const auto transaction = m_Connection->transaction();
                transaction->execute(query, exchange, accountID, profileID, currency, balance, available, hold, tradingEnabled);
                transaction->commit();
            }
            catch (const std::exception& exception) {
                spdlog::error("InsertAccount: {}", exception.what());
            }
            return 0;
        }



        std::uint64_t InsertOrder(const std::string& exchange, const std::string& orderID, const std::string& productID,
            const std::string& price, const std::string& orderSize, const std::string& orderType, const std::string& side,
            const std::string& stp, const std::string& timeInForce,
            bool postOnly, const std::string& createdAt, 
            const std::string& doneAt, 
            const std::string& doneReason,
            const std::string& funds,
            const std::string& specifiedFunds, 
            const std::string& fillFees, const std::string& filledSize,
            const std::string& executedValue, const std::string& status, bool settled) const
        {
            try {
                const auto query = R"(
                    INSERT INTO orders (
                        exchange,
                        order_id,
                        product_id,
                        price,
                        order_size,
                        order_type,
                        side,
                        stp,
                        time_in_force,
                        post_only,
                        created_at,
                        done_at,
                        done_reason,
                        funds,
                        specified_funds,
                        fill_fees,
                        filled_size,
                        executed_value,
                        status,
                        settled
                    ) VALUES (
                        $1, $2, $3, 
                        NULLIF($4,'')::DECIMAL, 
                        NULLIF($5,'')::DECIMAL, 
                        $6, $7, $8, $9, $10, $11, 
                        NULLIF($12,'')::TIMESTAMP WITH TIME ZONE,
                        $13,
                        NULLIF($14,'')::DECIMAL,
                        NULLIF($15,'')::DECIMAL, 
                        NULLIF($16,'')::DECIMAL, 
                        NULLIF($17,'')::DECIMAL, 
                        NULLIF($18,'')::DECIMAL, 
                        $19, 
                        $20
                    )
                )";
                const auto transaction = m_Connection->transaction();
                transaction->execute(query, exchange, orderID, productID,
                    price, orderSize, orderType, side,
                    stp, timeInForce,
                    postOnly, createdAt, doneAt, doneReason, funds, specifiedFunds, fillFees, filledSize,
                    executedValue, status, settled);
                transaction->commit();
            }
            catch (const std::exception& exception) {
                spdlog::error("InsertOrder: {}", exception.what());
            }
            return 0;
        }
    private:
        std::string m_Hostname;
        std::string m_Database;
        std::string m_Username;
        std::string m_Password;

        //pqxx::connection m_Connection;
        //PGconn* m_Connection;
    public:
        std::shared_ptr<tao::pq::connection> m_Connection;
    };

    cu::ref<PostgreSQL> DB;

}