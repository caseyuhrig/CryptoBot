#pragma once

#include <stdafx.hpp>


namespace CryptoAPI {


    struct CandleData
    {
        double Epoch;
        double Low;
        double High;
        double Open;
        double Close;

        CandleData() {}

        CandleData(const double low, const double high, const double open, const double close)
            : Low(low), High(high), Open(open), Close(close)
        {}
        ~CandleData() = default;
    };

    struct CandleRanges
    {
        double MinEpoch;
        double MaxEpoch;
        double MinPrice;
        double MaxPrice;
    };

    class ShouldBuy
    {
    public:
        ShouldBuy(const cu::ref<pqxx::connection> connection)
        {
            m_Connection = connection;
        }

        ~ShouldBuy() = default;
        
        inline void SetStartTime(const double startTime) { m_StartTime = startTime; };
        inline void SetDuration(const double duration) { m_Duration = duration; }

        double GetNow() {
            //if (m_Testing) return m_StartTime + m_Duration;
            //return time(0);
            return m_Testing ? m_StartTime + m_Duration : time(0);
        }

        inline double GetPrice()
        {
            return GetPriceFromDB(GetNow());
        }

        void Run()
        {
            const auto now = std::chrono::system_clock::from_time_t(GetNow());

            const auto twentySeconds = std::chrono::duration<double>(20);

            const auto newTime = now + twentySeconds;
            //twentySeconds.
            /*
                1) What is the price now?
            const auto currentPrice = GetPrice();
                2) was it higher or lower 5 minutes ago?
             
                3) was it higher or lower 15 minutes ago?

                4) At the current ROC, how long to make $10/$20?

                5) What has the currency been doing.
                    a) going up
                    b) going down
                    c) trading sideways
            */
            auto transaction = pqxx::work(*m_Connection);
            const auto rows = transaction.exec("SELECT * FROM view_graph");
            for (auto row : rows)
            {
                //min_price_enoch = row["min_price_epoch"].get<double>().value();
                //max_price_enoch = row["max_price_epoch"].get<double>().value();
                //min_price = row["min_price"].get<double>().value();
                //max_price = row["max_price"].get<double>().value();
            }
            transaction.commit();
        }

        std::vector<CandleData> GetCandleData()
        {

        }
    private:
        double GetPriceFromDB(const double someTime)
        {
            double result = 0;
            const std::string sql = fmt::format("select price from view_graph where price_time < to_timestamp({}) order by price_time desc limit 1", someTime);
            //spdlog::debug("{}", sql);
            auto txn3 = pqxx::work(*m_Connection);
            result = txn3.query_value<double>(sql);
            /*
            const auto r = txn.exec(sql);
            for (auto row : r)
            {
                current_price = row["price"].get<double>().value();
            }
            */
            txn3.commit();
            return result;
        }
    private:
        cu::ref<pqxx::connection> m_Connection;
        double m_StartTime;
        double m_Duration;
        double m_BuyPrice;
        double m_SellPrice;
        bool m_Testing = true;
    };
}