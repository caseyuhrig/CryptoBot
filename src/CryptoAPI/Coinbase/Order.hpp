#pragma once

#include <CryptoAPI/PostgreSQL.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <fmt/format.h>


namespace CryptoAPI {

    namespace Coinbase {

        struct OrderSpecification {
            std::int64_t ID;
            std::string Exchange;
            std::string OrderID;
            std::string ProductID;
            std::string Price;
            std::string OrderSize;
            std::string OrderType;
            std::string Side;
            std::string Stp;
            std::string TimeInForce;
            bool PostOnly;
            std::string CreatedAt;
            std::string DoneAt;
            std::string DoneReason;
            std::string Funds;
            std::string SpecifiedFunds;
            std::string FillFees;
            std::string FilledSize;
            std::string ExecutedValue;
            std::string Status;
            bool Settled;

            OrderSpecification(const nlohmann::json& json) 
                : ID(-1), Exchange("COINBASE"), PostOnly(false), Settled(false)
            {
                if (json.contains("id")) OrderID = json["id"];
                if (json.contains("product_id")) ProductID = json["product_id"];
                if (json.contains("price")) Price = json["price"];
                if (json.contains("size")) OrderSize = json["size"];
                if (json.contains("type")) OrderType = json["type"];
                if (json.contains("side")) Side = json["side"];
                if (json.contains("stp")) Stp = json["stp"];
                if (json.contains("time_in_force")) TimeInForce = json["time_in_force"];
                if (json.contains("post_only")) PostOnly = json["post_only"];
                if (json.contains("created_at")) CreatedAt = json["created_at"];
                if (json.contains("done_at")) DoneAt = json["done_at"];
                if (json.contains("done_reason")) DoneReason = json["done_reason"];
                if (json.contains("funds")) Funds = json["funds"];
                if (json.contains("specified_funds")) SpecifiedFunds = json["specified_funds"];
                if (json.contains("fill_fees")) FillFees = json["fill_fees"];
                if (json.contains("filled_size")) FilledSize = json["filled_size"];
                if (json.contains("executed_value")) ExecutedValue = json["executed_value"];
                if (json.contains("status")) Status = json["status"];
                if (json.contains("settled")) Settled = json["settled"];
            }
        };

        class Order
        {
        public:
            Order(const std::string& jsonText) : Order(nlohmann::json::parse(jsonText)) {}

            Order(const nlohmann::json& json) : m_Spec(json) 
            {
                // loads the ID from the DB if it already exists.
                m_Spec.ID = FindID(m_Spec.Exchange, m_Spec.OrderID);
            }

            ~Order() = default;

            const auto Get() const noexcept { return m_Spec; }
            const auto IsNew() const noexcept { return m_Spec.ID == -1; }

            /*
            Ref<Order> Create() const 
            {
                auto order = CreateRef<Order>();
            }
            */
            //void Place() const {}
            void Cancel() const {}

            void Save()
            {
                try {
                    if (IsNew()) Insert();
                    else Update();
                    Load();
                }
                catch (const std::exception& exception) {
                    spdlog::error("Order.Save: {}", exception.what());
                }
            }

            void Load()
            {
                spdlog::debug("Order.Load: ID = {}", m_Spec.ID);
                try {
                    if (m_Spec.ID <= 0)
                    {
                        const auto msg = fmt::format("Order.Select ID < 0, ID = {}", m_Spec.ID);
                        spdlog::error("ERR: {}", msg);
                        throw std::exception(msg.c_str());
                    }
                    const auto sql = std::string("SELECT * FROM orders WHERE id = $1");
                    const auto transaction = DB->m_Connection->transaction();
                    const auto result = transaction->execute(sql, m_Spec.ID);
                    //const auto size = result.size();
                    //spdlog::debug("Result Size: {}", result.size());
                    const auto row = 0;
                    //for (auto row = 0;row < result.size();row++)
                    //{
                    //auto v = result[0][0];
                        m_Spec.ID = std::stoul(result.get(row, result.index("id")));
                        m_Spec.Exchange = result.get(row, result.index("exchange"));
                        m_Spec.OrderID = result.get(row, result.index("order_id"));
                        m_Spec.ProductID = result.get(row, result.index("product_id"));
                        //m_Spec.Price = result[row][result.index("price")].get();
                        m_Spec.Price = Get(result, row, "price");
                        m_Spec.OrderSize = Get(result, row, "order_size");
                        m_Spec.OrderType = result.get(row, result.index("order_type"));
                        m_Spec.Side = result.get(row, result.index("side"));
                        m_Spec.Stp = result.get(row, result.index("stp"));
                        m_Spec.TimeInForce = result.get(row, result.index("time_in_force"));
                        m_Spec.PostOnly = result.get(row, result.index("post_only"));
                        m_Spec.CreatedAt = result.get(row, result.index("created_at"));
                        m_Spec.DoneAt = Get(result, row, "done_at");
                        m_Spec.DoneReason = Get(result, row, "done_reason");
                        m_Spec.Funds = Get(result, row, "funds");
                        m_Spec.SpecifiedFunds = Get(result, row, "specified_funds");
                        m_Spec.FillFees = Get(result, row, "fill_fees");
                        m_Spec.FilledSize = Get(result, row, "filled_size");
                        m_Spec.ExecutedValue = Get(result, row, "executed_value");
                        m_Spec.Status = Get(result, row, "status");
                        m_Spec.Settled = result.get(row, result.index("settled"));
                    //}
                    transaction->commit();
                }
                catch (const std::exception& exception) {
                    spdlog::error("Exception: Order::Load: {}", exception.what());
                }
            }

            const std::string Get(const tao::pq::result& result, const int row, const std::string& column) const
            {
                return !result.is_null(row, result.index(column)) ? result.get(row, result.index(column)) : "";
            }

            void InsertStub()
            {
                spdlog::debug("Order.InsertStub: {}", m_Spec.ID);
                try {
                    const auto query = std::string(R"(
                    INSERT INTO orders (
                        exchange,
                        order_id,
                        product_id,
                        price,
                        order_size,
                    )
                    )");
                    const auto transaction = DB->m_Connection->transaction();
                    transaction->execute(query, m_Spec.Exchange, m_Spec.OrderID, m_Spec.ProductID,
                        m_Spec.Price, m_Spec.OrderSize, m_Spec.OrderType, m_Spec.Side,
                        m_Spec.Stp, m_Spec.TimeInForce,
                        m_Spec.PostOnly, m_Spec.CreatedAt, m_Spec.DoneAt, m_Spec.DoneReason,
                        m_Spec.Funds, m_Spec.SpecifiedFunds,
                        m_Spec.FillFees, m_Spec.FilledSize,
                        m_Spec.ExecutedValue, m_Spec.Status, m_Spec.Settled);

                    m_Spec.ID = GetAutoIncrementID(transaction, "orders_id_seq");

                    transaction->commit();
                    Load();
                }
                catch (const std::exception& exception) {
                    spdlog::error("Exception: Order.InsertStub: {}", exception.what());
                }
            }


            void Insert()
            {
                spdlog::debug("Order.Insert: {}", m_Spec.ID);
                try {
                    const auto query = std::string(R"(
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
                    )");
                    const auto transaction = DB->m_Connection->transaction();
                    transaction->execute(query, m_Spec.Exchange, m_Spec.OrderID, m_Spec.ProductID,
                        m_Spec.Price, m_Spec.OrderSize, m_Spec.OrderType, m_Spec.Side,
                        m_Spec.Stp, m_Spec.TimeInForce,
                        m_Spec.PostOnly, m_Spec.CreatedAt, m_Spec.DoneAt, m_Spec.DoneReason, 
                        m_Spec.Funds, m_Spec.SpecifiedFunds,
                        m_Spec.FillFees, m_Spec.FilledSize,
                        m_Spec.ExecutedValue, m_Spec.Status, m_Spec.Settled);

                    m_Spec.ID = GetAutoIncrementID(transaction, "orders_id_seq");
                    
                    transaction->commit();
                    Load();
                }
                catch (const std::exception& exception) {
                    spdlog::error("Exception: Order.Insert: {}", exception.what());
                }
            }

            void Update() const
            {
                spdlog::debug("Order.Update: {}", m_Spec.ID);
                try {
                    const auto query = std::string(R"(
                    UPDATE orders SET
                        done_at = $1,
                        done_reason = $2,
                        funds = NULLIF($3,'')::DECIMAL,
                        specified_funds = NULLIF($4,'')::DECIMAL,
                        fill_fees = NULLIF($5,'')::DECIMAL,
                        filled_size = NULLIF($6,'')::DECIMAL,
                        executed_value = NULLIF($7,'')::DECIMAL,
                        status = $8,
                        settled = $9
                    WHERE
                        id = $10
                    )");
                    const auto transaction = DB->m_Connection->transaction();
                    transaction->execute(query, 
                        m_Spec.DoneAt,
                        m_Spec.DoneReason,
                        m_Spec.Funds,
                        m_Spec.SpecifiedFunds,
                        m_Spec.FillFees,
                        m_Spec.FilledSize,
                        m_Spec.ExecutedValue,
                        m_Spec.Status,
                        m_Spec.Settled,
                        m_Spec.ID);
                    transaction->commit();
                }
                catch (const std::exception& exception) {
                    spdlog::error("Order.Update: {}", exception.what());
                }
            }
            /*
            void Copy(const Order& order)
            {
                m_ID = order.GetID();
                m_OrderID = order.GetOrderID();
            }
            */

        private:
            OrderSpecification m_Spec;

            static const std::int64_t GetAutoIncrementID(const cu::ref<tao::pq::transaction<tao::pq::parameter_text_traits>>& transaction, 
                const std::string& sequence_name)
            {
                std::int64_t sequence = -1;
                auto sql = fmt::format("SELECT currval('{}')", sequence_name);
                const auto results = transaction->execute(sql);
                const auto size = results.size();
                spdlog::debug("GetAutoIncrementID size: {}", size);
                if (size > 0)
                    sequence = std::stoul(results.get(0, 0));
                spdlog::debug("ID: {}", sequence);
                return sequence;
            }
        
            static const std::int64_t FindID(const std::string& exchange, const std::string& orderID)
            {
                std::int64_t id = -1;
                const std::string query = "SELECT id FROM orders WHERE exchange = $1 AND order_id = $2";
                const auto transaction = DB->m_Connection->transaction();
                const auto result = transaction->execute(query, exchange, orderID);
                const auto size = result.size();
                if (size > 0)
                {
                    id = std::stoul(result.get(0, 0));
                }
                transaction->commit();
                return id;
            }


        };
    }
}