/**
* (c) 2021, Casey Uhrig
*/
#include <stdafx.hpp>

#include <CryptoAPI/Coinbase/CoinbaseEx.hpp>
#include <CryptoAPI/MarketData/products.hpp>
#include <CryptoAPI/WebSocket.hpp>
#include <CryptoAPI/UI.hpp>
#include <CryptoAPI/CandleStickChart.hpp>

#include <cu/common.hpp>


namespace MyImPlot {
void PlotCandlestick(const char* label_id, const double* xs, const double* opens, const double* closes, const double* lows, const double* highs, int count, bool tooltip = true, float width_percent = 0.25f, ImVec4 bullCol = ImVec4(0, 1, 0, 1), ImVec4 bearCol = ImVec4(1, 0, 0, 1));
}

// https://www.reddit.com/r/VisualStudio/comments/iswdpm/cmake_and_visual_studio_2019/
// http://goorep.se:1001/changelog/report/rSelect/PAGE_result.htm?alias=guest6&set=api&query=Book+pages&$$TArticleBook1.ArticleBookK=7107&link=%5B%5B%229F1E006D78894848838A0970E2FF0BE9zoom%22,%22Object1%22,7107%5D,%5B%2271C91DEE3C5A4FDC8EC1114C7C18033Bzoom%22,%22TArticleBook1%22,7107%5D%5D&rows=25
// http://goorep.se:1001/changelog/report/rSelect/PAGE_result.htm?alias=guest7&set=api&query=Book%20pages&$$TArticleBook1.ArticleBookK=7116&link=%5B%5B%229F1E006D78894848838A0970E2FF0BE9zoom%22,%22Object1%22,7116%5D,%5B%2271C91DEE3C5A4FDC8EC1114C7C18033Bzoom%22,%22TArticleBook1%22,7116%5D%5D&rows=25

// https://www.youtube.com/watch?v=nlKcXPUJGwA


int main(int argc, char** argv)
{
    try {
        spdlog::set_level(spdlog::level::trace);

        spdlog::info("CryptoBot v0.1");

        std::string program = argv[0];
        std::vector args = std::vector(argv + 1, argv + argc);

        spdlog::info("Program: {} Args: {}", program, args.size());
        for (int n = 0;n < args.size();n++)
        {
            spdlog::info("  Arg[{}]: {}", n, args[n]);
        }


        const auto properties_path = cu::user::home_path().append(".crypto-bot").append("crypto-bot.properties");

        spdlog::info("Properties: {}", properties_path.string());

        auto properties = cu::properties::load(properties_path);
        for (const auto& entry : properties)
        {
            spdlog::info("{} = {}", entry.first, entry.second);
        }

        // PostgreSQL Database --------------------------

        const auto DB_HOSTNAME = properties["postgresql.hostname"];
        const auto DB_PORT = properties["postgresql.port"];
        const auto DB_DATABASE = properties["postgresql.database"];
        const auto DB_USERNAME = properties["postgresql.username"];
        const auto DB_PASSWORD = properties["postgresql.password"];

        // Coinbase -------------------------------------

        const auto CB_WS_HOSTNAME = properties["coinbase_pro.websocket.hostname"];
        const auto CB_WS_PORT = properties["coinbase_pro.websocket.port"];
        
        const auto API_KEY = properties["coinbase_pro.api_key"];
        const auto API_SECRET = properties["coinbase_pro.api_secret"];
        const auto PASSPHRASE = properties["coinbase_pro.passphrase"];
        const auto mode = CryptoAPI::Coinbase::Auth::Mode::LIVE; //SANDBOX;

        CryptoAPI::DB = CryptoAPI::PostgreSQL::Create(DB_HOSTNAME, DB_DATABASE, DB_USERNAME, DB_PASSWORD);
        CryptoAPI::DB->Connect();

        const auto auth = CryptoAPI::Coinbase::Auth(API_KEY, API_SECRET, PASSPHRASE, mode);

        const auto coinbase = CryptoAPI::Coinbase::CoinbaseEx(auth);

        // Program ----------------------------------------

        CryptoAPI::UI::Initialize();

        auto c = pqxx::connection("postgresql://crypto:crypto@localhost/crypto");

        struct xy_point {
            double epoch;
            double price;
        };

        struct xy_limits {
            double min_price_epoch;
            double max_price_epoch;
            double min_price;
            double max_price;
        };

        /*
        std::vector<xy_point> xy_data;
        std::vector<double> x_data;
        std::vector<double> y_data;
        xy_limits xy_data_limits(0.0);

        std::vector<CryptoAPI::CandleData> candle_data;
        std::vector<double> dates;
        std::vector<double> lows;
        std::vector<double> highs;
        std::vector<double> opens;
        std::vector<double> closes;
        CryptoAPI::CandleRanges candle_ranges(0.0);

        std::thread qthread1;
        std::thread qthread2;
        std::thread qthread3;
        std::thread qthread4;

        auto done1 = false;
        auto done2 = false;
        auto done3 = false;
        auto done4 = false;
        */

        /*
        qthread1 = std::thread([&qthread1, &done1, &xy_data, &x_data, &y_data] {
            spdlog::debug("Starting query thread1...");
            {
                auto conn = pqxx::connection("postgresql://crypto:crypto@localhost/crypto");
                xy_data = cu::query<xy_point>(conn, "SELECT * FROM view_graph",
                    [](const pqxx::row& row) -> xy_point {
                        auto data = xy_point();
                        data.epoch = row["price_epoch"].get<double>().value();
                        data.price = row["price"].get<double>().value();
                        return data;
                    });
                x_data = cu::vmap([](const xy_point& xy) { return xy.epoch; }, xy_data);
                y_data = cu::vmap([](const xy_point& xy) { return xy.price; }, xy_data);
                spdlog::debug("Done with query thread1.");
                qthread1.detach();
                done1 = true;
            }
        });

        qthread2 = std::thread([&qthread2, &done2, &xy_data_limits] {
            spdlog::debug("Starting query thread2...");
            {
                auto conn = pqxx::connection("postgresql://crypto:crypto@localhost/crypto");
                xy_data_limits = cu::query<xy_limits>(conn, "SELECT * FROM view_graph_limits",
                    [](const pqxx::row& row) -> xy_limits {
                        auto data = xy_limits();
                        data.min_price_epoch = row["min_price_epoch"].get<double>().value();
                        data.max_price_epoch = row["max_price_epoch"].get<double>().value();
                        data.min_price = row["min_price"].get<double>().value();
                        data.max_price = row["max_price"].get<double>().value();
                        return data;
                    })[0];
            }
            spdlog::debug("Done with query thread2.");
            qthread2.detach();
            done2 = true;
        });
        */


        const auto xy_data = cu::query<xy_point>(c,
            "SELECT * FROM view_graph_m",
            [](const pqxx::row& row) -> xy_point {
                auto data = xy_point();
                data.epoch = row["price_epoch"].get<double>().value();
                data.price = row["price"].get<double>().value();
                return data;
            });


        const auto x_data = cu::vmap([](const xy_point& xy) { return xy.epoch; }, xy_data);
        const auto y_data = cu::vmap([](const xy_point& xy) { return xy.price; }, xy_data);


        const auto xy_data_limits = cu::query<xy_limits>(c,
            "SELECT * FROM view_graph_limits_m",
            [](const pqxx::row& row) -> xy_limits {
                auto data = xy_limits();
                data.min_price_epoch = row["min_price_epoch"].get<double>().value();
                data.max_price_epoch = row["max_price_epoch"].get<double>().value();
                data.min_price = row["min_price"].get<double>().value();
                data.max_price = row["max_price"].get<double>().value();
                return data;
            })[0];

        /*
        qthread3 = std::thread([&qthread3, &done3, &candle_data, &dates, &lows, &highs, &opens, &closes] {
            spdlog::debug("Starting query thread3...");
            {
                auto conn = pqxx::connection("postgresql://crypto:crypto@localhost/crypto");

                candle_data = cu::query<CryptoAPI::CandleData>(conn, "SELECT * FROM chart_candlestick_hourly",
                [](const pqxx::row& row) -> CryptoAPI::CandleData {
                    auto data = CryptoAPI::CandleData();
                    data.Epoch = row["hour_epoch"].get<double>().value();
                    data.Low = row["low"].get<double>().value();
                    data.High = row["high"].get<double>().value();
                    data.Open = row["open"].get<double>().value();
                    data.Close = row["close"].get<double>().value();
                    return data;
                });

                dates = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Epoch; }, candle_data);
                lows = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Low; }, candle_data);
                highs = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.High; }, candle_data);
                opens = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Open; }, candle_data);
                closes = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Close; }, candle_data);

                spdlog::debug("Done with query thread3.");
                qthread3.detach();

                done3 = true;
            }
        });



        qthread4 = std::thread([&qthread4, &done4, &candle_ranges] {
            spdlog::debug("Starting query thread4...");
            {
                auto conn = pqxx::connection("postgresql://crypto:crypto@localhost/crypto");
                candle_ranges = cu::query<CryptoAPI::CandleRanges>(conn,
                    "SELECT * FROM chart_candlestick_hourly_ranges",
                    [](const pqxx::row& row) -> CryptoAPI::CandleRanges {
                        auto data = CryptoAPI::CandleRanges();
                        data.MinEpoch = row["min_hour_epoch"].get<double>().value();
                        data.MaxEpoch = row["max_hour_epoch"].get<double>().value();
                        data.MinPrice = row["min_price"].get<double>().value();
                        data.MaxPrice = row["max_price"].get<double>().value();
                        return data;
                    })[0];
            }
            spdlog::debug("Done with query thread4.");
            qthread4.detach();

            done4 = true;
        });
        */



        const auto candle_data = cu::query<CryptoAPI::CandleData>(c, "SELECT * FROM chart_candlestick_hourly_m",
            [](const pqxx::row& row) -> CryptoAPI::CandleData {
                auto data = CryptoAPI::CandleData();
                data.Epoch = row["hour_epoch"].get<double>().value();
                data.Low = row["low"].get<double>().value();
                data.High = row["high"].get<double>().value();
                data.Open = row["open"].get<double>().value();
                data.Close = row["close"].get<double>().value();
                return data;
            }
        );

        const auto dates = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Epoch; }, candle_data);
        const auto lows = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Low; }, candle_data);
        const auto highs = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.High; }, candle_data);
        const auto opens = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Open; }, candle_data);
        const auto closes = cu::vmap([](const CryptoAPI::CandleData& cd) { return cd.Close; }, candle_data);

        const auto candle_ranges = cu::query<CryptoAPI::CandleRanges>(c, "SELECT * FROM chart_candlestick_hourly_limits_m",
        [](const pqxx::row& row) -> CryptoAPI::CandleRanges {
            auto data = CryptoAPI::CandleRanges();
            data.MinEpoch = row["min_hour_epoch"].get<double>().value();
            data.MaxEpoch = row["max_hour_epoch"].get<double>().value();
            data.MinPrice = row["min_price"].get<double>().value();
            data.MaxPrice = row["max_price"].get<double>().value();
            return data;
        })[0];





        // imgui needs a current_time float down below and not sure how to cast this w/ c++ properly.
        float current_time = xy_data_limits.min_price_epoch;
        auto current_price = 1.8f;

        std::vector<double> buy_time;
        std::vector<double> buy_price;

        buy_time.push_back(1620802801.440439);
        //buy_time.push_back(0.0);

        buy_price.push_back(2.0);

        bool selected[10] = {};
        // Header
        struct ColumnHeader
        {
            const char* label;
            float size;
        };

        ColumnHeader headers[] =
        {
            { "Idx",        -1 },
            { "Name",       150 },
            { "RT",         -1 },
        };

        std::thread th;

        while (CryptoAPI::UI::Loop())
        {
            CryptoAPI::UI::ProcessEvents();
            {
                ImGui::Begin("CryptoBot");
                ImGui::Text("24/7 crypto trading!");
                if (ImGui::BeginTable("split1", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
                {
                    //ImGui::Columns(3, "split1", true);
                    //ImGui::Separator();
                    for (int i = 0; i < 10; i++)
                    {
                        char label[32];
                        sprintf_s(label, "Item %d", i);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Selectable(label, &selected[i]); // FIXME-TABLE: Selection overlap
                        ImGui::TableNextColumn();
                        ImGui::Text("a123456");
                        ImGui::TableNextColumn();
                        ImGui::Text("b123456");
                    }
                    ImGui::EndTable();
                }
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                if (ImGui::Button("Place Order")) {
                    try {
                        coinbase.PlaceOrder("ADA-USD", "1", "1.4901");
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Backup: {}", exception.what());
                    }
                }
                if (ImGui::Button("List/Save/Sync Orders")) {
                    try {
                        auto orders = coinbase.SyncOrders();
                        for (auto& order : orders)
                        {
                            spdlog::info("{}\t{}\t{}\t{}\t{}", order.Get().ID, order.Get().OrderID,
                                order.Get().ProductID, order.Get().Price, order.Get().Status);
                        }
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Backup: {}", exception.what());
                    }
                }
                if (ImGui::Button("List Accounts")) {
                    try {
                        auto accounts = coinbase.GetTradingAccounts();
                        for (auto& account : accounts) {
                            spdlog::info("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
                                account.GetID(), account.GetExchange(), account.getProfileID(), account.GetAccountID(),
                                account.GetCurrency(),
                                account.GetBalance(), account.GetAvailable(), account.GetHold(), account.GetTradingEnabled());
                            account.Save();
                        }
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("List Accounts: {}", exception.what());
                    }
                }
                if (ImGui::Button("List Currencies")) {
                    try {
                        const auto pairs = coinbase.GetCurrencyPairs();
                        //const auto currencies = coinbase.GetCurrencies();
                        for (const auto& pair : pairs)
                        {
                            spdlog::info(pair.GetIdentifier());
                        }
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("List Currencies: {}", exception.what());
                    }
                }
                if (ImGui::Button("Populate Trading Pairs")) {
                    try {
                        const auto pairs = coinbase.GetCurrencyPairs();
                        //const auto currencies = coinbase.GetCurrencies();
                        for (const auto& pair : pairs)
                        {
                            spdlog::info(pair.GetIdentifier());
                            pair.Save();
                        }
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Populate Trading Pairs: {}", exception.what());
                    }
                }
                if (ImGui::Button("Download Ticker")) {

                    try {
                        th = std::thread([CB_WS_HOSTNAME, CB_WS_PORT] {
                            try {
                                const auto web = CryptoAPI::TickerWebSocket(CB_WS_HOSTNAME, CB_WS_PORT);
                                web.FixMe();
                                //const auto tws = CryptoAPI::TickerWebSocket(CB_WS_HOSTNAME, CB_WS_PORT);
                                //tws.Run();
                                spdlog::info("After test...");
                            }
                            catch (const std::exception& exception) {
                                spdlog::critical("{}", exception.what());
                            }
                            });
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Download Ticker: {}", exception.what());
                    }

                }
                if (ImGui::Button("Download BTC/USD Ticker")) {
                    try {
                        auto ticker = CryptoAPI::Coinbase::marketdata::products::getTicker(auth, "BTC-USD");
                        spdlog::info("BTC-USD ($): {}", ticker.getPrice());
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Download BTC/USD Ticker: {}", exception.what());
                    }
                }
                if (ImGui::Button("Download Orderbook Level 3")) {
                    try {
                        auto book = CryptoAPI::Coinbase::marketdata::products::getOrderBookLevelThree(auth, "BTC-USD");
                        auto bids = book.getBids();
                        for (auto& bid : bids) {
                            if (std::stoi(bid.getSize()) > 5) {
                                std::cout << bid.getOrderId() << std::endl;
                            }
                        }
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Download Orderbook Level 3: {}", exception.what());
                    }
                }
                if (ImGui::Button("Create Graph")) {
                    try {
                        CryptoAPI::DB->CreateGraph();
                    }
                    catch (const std::exception& exception) {
                        spdlog::critical("Create Graph: {}", exception.what());
                    }
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Coin Trading Prices");
                //ImPlot::SetNextPlotLimits(0.001, 761.623, 1.5, 2.1);
                ImPlot::SetNextPlotLimits(xy_data_limits.min_price_epoch, xy_data_limits.max_price_epoch, xy_data_limits.min_price - 0.5, xy_data_limits.max_price + 0.5);
                //ImVec2(400, 300)
                //ImVec2(400, 300)
                // ImPlotFlags_Crosshairs
                if (ImPlot::BeginPlot("Crypto", "Time", "Price", ImVec2(-1, -1), ImPlotFlags_Crosshairs, ImPlotAxisFlags_Time)) {

                    //ImPlot::PlotBars("My Bar Plot", bar_data, 11);
                    ImPlot::PlotLine("ADA", x_data.data(), y_data.data(), x_data.size());
                    ImPlot::PlotScatter("Buy", buy_time.data(), buy_price.data(), buy_time.size());
                    ImPlot::Annotate(buy_time[0], buy_price[0], ImVec2(10, 10), ImPlot::GetLastItemColor(), "$20");
                    ImPlot::Annotate(current_time, current_price, ImVec2(10, 10), ImPlot::GetLastItemColor(), "Start");

                    ImPlot::EndPlot();
                }
                ImGui::End();
            }
            {
                ImGui::Begin("ADA Trading Candlestick");
                ImGui::BulletText("You can create custom plotters or extend ImPlot using implot_internal.h.");
                static bool tooltip = true;
                static ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
                static ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);
                ImGui::SameLine(); ImGui::ColorEdit4("##Bull", &bullCol.x, ImGuiColorEditFlags_NoInputs);
                ImGui::SameLine(); ImGui::ColorEdit4("##Bear", &bearCol.x, ImGuiColorEditFlags_NoInputs);
                //ImPlot::GetStyle().UseLocalTime = false;
                //ImPlot::SetNextPlotFormatY("$%.0f"); //, ImPlotYAxis_1);
                ImPlot::SetNextPlotLimits(candle_ranges.MinEpoch, candle_ranges.MaxEpoch, candle_ranges.MinPrice - 0.5, candle_ranges.MaxPrice + 0.5);
                if (ImPlot::BeginPlot("Candlestick Chart", NULL, NULL, ImVec2(-1, -1), 0, ImPlotAxisFlags_Time)) { // ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit)) {
                    MyImPlot::PlotCandlestick("ADA", dates.data(), opens.data(), closes.data(), lows.data(), highs.data(), dates.size(), tooltip, 0.25f, bullCol, bearCol);
                    ImPlot::EndPlot();
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Controls");
                if (ImGui::Button("Run")) {
                    spdlog::info("Run <pressed>");
                }
                if (ImGui::SliderFloat("Time", &current_time, static_cast<float>(xy_data_limits.min_price_epoch), static_cast<float>(xy_data_limits.max_price_epoch)))
                {
                    const auto sql = fmt::format("select price from view_graph_m where price_time < to_timestamp({}) order by price_time desc limit 1", current_time);
                    current_price = cu::query_value<double>(c, sql).value();
                }
                ImGui::End();
            }
            ImGui::ShowDemoWindow();
            ImPlot::ShowDemoWindow();
            CryptoAPI::UI::Render();
        }
        CryptoAPI::UI::Shutdown();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception) {
        spdlog::critical("BOOM: {}", exception.what());
    }
    return EXIT_FAILURE;
}
