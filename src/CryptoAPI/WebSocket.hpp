#pragma once

#include <CryptoAPI/PostgreSQL.hpp>

//#include "example/common/root_certificates.hpp"

//#define BOOST_ASIO_NO_DEPRECATED

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/require.hpp>
/*
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
*/
#include <spdlog/spdlog.h>
//#include "root_certificates.hpp"
#include <nlohmann/json.hpp>

#include <cu/common.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <future>


namespace CryptoAPI {

    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = beast::http;           // from <boost/beast/http.hpp>
    namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


    static void fail(beast::error_code ec, char const* what)
    {
        spdlog::error("{}[{}]: {}", what, ec.value(), ec.message());
    }

    // Sends a WebSocket message and prints the response
    class Session : public std::enable_shared_from_this<Session>
    {
    private:
        tcp::resolver m_Resolver;
        websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
        beast::flat_buffer buffer_;
        std::string host_;
        std::string text_;
    public:
        // Resolver and socket require an io_context
        explicit Session(net::io_context& ioc, ssl::context& ctx)
            : m_Resolver(net::make_strand(ioc)), ws_(net::make_strand(ioc), ctx)
        {
            spdlog::debug("WebScocket Session Constructor");          
        }

        // Start the asynchronous operation
        void run(const std::string& host, const std::string& port, const std::string& text)
        {
            spdlog::debug("run");
            // Save these for later
            host_ = host;
            text_ = text;
            // Look up the domain name
            m_Resolver.async_resolve(host, port, beast::bind_front_handler(&Session::on_resolve, shared_from_this()));
        }

        void on_resolve(beast::error_code ec, tcp::resolver::results_type results)
        {
            spdlog::debug("on_resolve");

            if (ec)
                return fail(ec, "resolve");

            

            // Set a timeout on the operation
            beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

            // Make the connection on the IP address we get from a lookup
            beast::get_lowest_layer(ws_).async_connect(results, beast::bind_front_handler(&Session::on_connect,
                    shared_from_this()));
        }

        void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
        {
            spdlog::debug("on_connect");

            if (ec)
                return fail(ec, "connect");

            

            // Update the host_ string. This will provide the value of the
            // Host HTTP header during the WebSocket handshake.
            // See https://tools.ietf.org/html/rfc7230#section-5.4
            //host_ += ':' + std::to_string(ep.port());

            // Set a timeout on the operation
            beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

            // Set SNI Hostname (many hosts need this to handshake successfully)
            if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
            {
                boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
                throw boost::system::system_error{ ec };
            }

            // Perform the SSL handshake
            ws_.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&Session::on_ssl_handshake,
                    shared_from_this()));
        }

        void on_ssl_handshake(beast::error_code ec)
        {
            spdlog::debug("on_ssl_handshake");

            if (ec)
                return fail(ec, "ssl_handshake");

            

            // Turn off the timeout on the tcp_stream, because
            // the websocket stream has its own timeout system.
            beast::get_lowest_layer(ws_).expires_never();

            // Set suggested timeout settings for the websocket
            ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
            auto h = host_;
            // Set a decorator to change the User-Agent of the handshake
            ws_.set_option(websocket::stream_base::decorator(
                [h](websocket::request_type& req)
                {
                    req.set(http::field::host, h);
                    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                    req.set(http::field::content_type, "application/json");
                }));

            // Perform the websocket handshake
            ws_.async_handshake(host_, "/", beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
        }

        void on_handshake(beast::error_code ec)
        {
            spdlog::debug("on_handshake");

            if (ec)
                return fail(ec, "handshake");

            auto const rpcJson = R"({
    "type": "subscribe",
    "product_ids": [
        "ETH-USD",
        "ETH-EUR"
    ],
    "channels": [
        "level2",
        "heartbeat",
        {
            "name": "ticker",
            "product_ids": [
                "ETH-BTC",
                "ETH-USD"
            ]
        }
    ]
})";
            auto const rpcJson2 = R"({
    "type": "subscribe",
    "product_ids": [
        "GRT-USD"
    ],
    "channels": [
        "level2",
        "heartbeat",
        {
            "name": "ticker",
            "product_ids": [
                "GRT-USD"
            ]
        }
    ]
})";         
            auto const rpcJson3 = R"({
    "type": "subscribe",
    "product_ids": [
        "GRT-USD"
    ],
    "channels": [
        "heartbeat",
        {
            "name": "ticker",
            "product_ids": [
                "GRT-USD"
            ]
        }
    ]
})";
            //auto pairList = std::string("\"GRT-USD\", \"ADA-USD\"");

            auto const rpcJson44 = R"({
    "type": "subscribe",
    "product_ids": [
        "MKR-USD","UNI-USD","NU-USD","UMA-USD","SUSHI-USD","GRT-USD","FIL-USD","YFI-USD","LRC-USD","EOS-USD","BAND-USD","CRV-USD","ETC-USD","NKN-USD","XLM-USD","REP-USD","BTC-USD","OXT-USD","BCH-USD","ZRX-USD","NMR-USD","ZEC-USD","COMP-USD","AAVE-USD","BNT-USD","ANKR-USD","ALGO-USD","ETH-USD","WBTC-USD","BAL-USD","DAI-USD","KNC-USD","STORJ-USD","SNX-USD","OMG-USD","CGLD-USD","ADA-USD","ATOM-USD","SKL-USD","MATIC-USD","OGN-USD","1INCH-USD","LTC-USD","LINK-USD","XTZ-USD","DASH-USD","REN-USD","ENJ-USD"
    ],
    "channels": [
        "ticker"
    ]
})";
            //const auto pairList = m_PG.GetPairList();
            //spdlog::info("PAIR LIST: {}", pairList);
            //auto const rpcJson4 = "({" + fmt::format(" \"type\": \"subscribe\", \"product_ids\": [ {} ], \"channels\": [ \"ticker\" ]", pairList) + "})";
            //ws.write(net::buffer(std::string(rpcJson)));
            //spdlog::debug("JSON: {}", rpcJson4);
            // Send the message
            ws_.async_write(net::buffer(std::string(rpcJson44)), beast::bind_front_handler(&Session::on_write, shared_from_this()));
        }

        // -------------------------------------
        // ADDED CONST to the two parameters
        // -------------------------------------
        void on_write(const beast::error_code ec, [[maybe_unused]] const std::size_t bytes_transferred)
        {
            spdlog::debug("on_write");

            //boost::ignore_unused(bytes_transferred);

            if (ec)
                return fail(ec, "write");

            // Read a message into our buffer
            ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
        }

        void on_read(const beast::error_code& ec, [[maybe_unused]] const std::size_t& bytes_transferred)
        {
            try {
                //spdlog::debug("on_read");

                if (ec) {
                    //fail(ec, "read2");
                    spdlog::critical("{}[{}]: {}", "read", ec.value(), ec.message());

                    //m_Resolver.cancel();

                    // Close the WebSocket connection
                    //ws_.async_close(websocket::close_code::normal, beast::bind_front_handler(&Session::on_close, shared_from_this()));
                    return;
                    //return fail(ec, "read");
                    //return;
                }
                else {
                    // return fail(ec, "read");

                    const auto buf = boost::beast::buffers_to_string(buffer_.data());

                    //spdlog::debug(buf);

                    const auto json = nlohmann::json::parse(buf);
                    //auto js2 = boost::json::parse(buf);
                    //auto& obj = js2.as_object();
                    //auto t2 = std::string(obj.at("type").as_string().c_str());

                    if (!json.contains("type"))
                    {
                        spdlog::critical("CANNOT FIND TYPE");
                        spdlog::debug(buf);
                    }
                    const auto type = json["type"].get<std::string>();

                    if (type == "subscriptions")
                    {
                        spdlog::debug(buf);
                    }
                    else if (type == "ticker")
                    {
                        const auto sequence = json["sequence"].get<std::uint64_t>();
                        const auto product_id = json["product_id"].get<std::string>();
                        const auto price = json["price"].get<std::string>();
                        const auto price_time = json["time"].get<std::string>();
                        const auto exchange_trade_id = json["trade_id"].get<std::uint64_t>();
                        const auto last_size = json["last_size"].get<std::string>();
                        const auto side = json["side"].get<std::string>();
                        const auto open_24h = json["open_24h"].get<std::string>();
                        const auto volume_24h = json["volume_24h"].get<std::string>();
                        const auto low_24h = json["low_24h"].get<std::string>();
                        const auto high_24h = json["high_24h"].get<std::string>();
                        const auto volume_30d = json["volume_30d"].get<std::string>();
                        const auto best_bid = json["best_bid"].get<std::string>();
                        const auto best_ask = json["best_ask"].get<std::string>();



                        DB->InsertTickerAmount(product_id, sequence, price, price_time, exchange_trade_id,
                            last_size, side, open_24h, volume_24h, low_24h, high_24h, volume_30d, best_bid, best_ask);
                        //auto& boostString = obj.at("sequence").as_string();
                        //auto seq = std::string(boostString.begin(), boostString.end());
                        /*
                        spdlog::info("---------------------------");
                        spdlog::info("      type: {}", type);
                        spdlog::info("  sequence: {}", sequence);
                        spdlog::info("product_id: {}", productID);
                        spdlog::info("     price: {}", price);
                        */
                        //--> spdlog::info("{}\t{}\t{}\t{}", type, sequence, productID, price);
                        //spdlog::info("---------------------------");
                    }
                    else {
                        spdlog::error("UNKNOWN TYPE: {}", type);
                        spdlog::debug(buf);
                        spdlog::debug(json.dump(2, ' '));
                    }

                    // Close the WebSocket connection
                    //ws_.async_close(websocket::close_code::normal, beast::bind_front_handler(&Session::on_close, shared_from_this()));
                }
                // we still want to continue even if there is an error, at least for now!
                buffer_.clear();

                //here I try to read the buffer again instead of ccalling the async_close.
                ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
            }
            catch (const std::exception& exception) 
            {
                spdlog::critical("Exception: onRead: {}", exception.what());
            }
        }

        void on_close(beast::error_code ec)
        {
            spdlog::debug("on_close");
            if (ec)
                return fail(ec, "close");

            // If we get here then the connection is closed gracefully

            // The make_printable() function helps print a ConstBufferSequence
            //std::cout << beast::make_printable(buffer_.data()) << std::endl;
        }
    };


    class TickerWebSocket
    {
    private:
        std::string m_Hostname;
        std::string m_Port;
        cu::ref<Session> m_Session;
        //boost::asio::io_context m_IO_Context;    // ioc, io_context
        //boost::asio::ssl::context m_SSL_Context; // ctx, ssl_context
    public:

        TickerWebSocket(const std::string& hostname, const std::string& port) 
            : m_Hostname(hostname), m_Port(port)
        {
            try {
                //m_IO_Context = boost::asio::io_context();
                //m_SSL_Context = boost::asio::ssl::context(boost::asio::ssl::context::sslv23_client);
                //m_SSL_Context.set_default_verify_paths();
                //m_Session = CreateRef<Session>(&m_IO_Context, &m_SSL_Context);
            }
            catch (const std::exception& exception) {
                spdlog::error("TWS(constructor): {}", exception.what());
            }
        }
        
        ~TickerWebSocket() = default;
        /*
        void Run() const
        {
            try {
                auto io_context = boost::asio::io_context();
                auto ssl_context = boost::asio::ssl::context(boost::asio::ssl::context::sslv23_client);
                m_Session = CreateRef<Session>(io_context, ssl_context);
                m_Session->run(m_Hostname, m_Port, "");
                io_context.run();
            }
            catch (const std::exception& exception) {
                spdlog::error("TWS.Run: {}", exception.what());
            }
        }
        
        void Shutdown() const
        {
            try {
                m_IO_Context->stop();
            }
            catch (const std::exception& exception) {
                spdlog::error("TWS.Shutdown: {}", exception.what());
            }
        }
        */
        void FixMe() const
        {
            //auto const host = "ws-feed.pro.coinbase.com";
            //auto const port = "443";
            auto const text = "";

            // The io_context is required for all I/O
            auto ioc = boost::asio::io_context();

            // The SSL context is required, and holds certificates
            auto ctx = boost::asio::ssl::context(ssl::context::sslv23_client);
            //ctx = std::make_shared<ssl::context>(ssl::context::sslv23);
            ctx.set_default_verify_paths();

            // This holds the root certificate used for verification
            //load_root_certificates(ctx);

            // Launch the asynchronous operation
            const auto session = cu::create<Session>(ioc, ctx);
            session->run(m_Hostname, m_Port, text);
            
            // abort error info
            // https://stackoverflow.com/questions/50693708/boost-asio-ssl-not-able-to-receive-data-for-2nd-time-onwards-1st-time-ok
            
            // Run the I/O service. The call will return when
            // the socket is closed.
            ioc.run();

            spdlog::info("Web Exiting...");
        }

/*
        class Web
        {
            //private:
            //    Ref<Session> m_Session;
        public:

            Web() {}
            ~Web() = default;

            void TestWebSockets() const
            {
                auto const host = "ws-feed.pro.coinbase.com";
                auto const port = "443";
                auto const text = "";

                // The io_context is required for all I/O
                auto ioc = boost::asio::io_context();

                // The SSL context is required, and holds certificates
                auto ctx = ssl::context(ssl::context::sslv23_client);
                //ctx = std::make_shared<ssl::context>(ssl::context::sslv23);
                ctx.set_default_verify_paths();

                // This holds the root certificate used for verification
                //load_root_certificates(ctx);

                // Launch the asynchronous operation
                const auto session = CreateRef<Session>(ioc, ctx);
                session->run(host, port, text);

                //auto work = boost::asio::require(ioc.get_executor(),
                //    boost::asio::execution::outstanding_work.tracked);

                //boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work =
                //auto work = boost::asio::make_work_guard(&ioc);
                //work.reset();

                //try {
                    //std::async(std::launch::async, std::bind(&boost::asio::io_context::run, &ioc));
                    //boost::bind(&boost::asio::io_context::run, &ioc);
                    //std::thread(boost::bind(&boost::asio::io_context::run, &ioc));
                //}
                //catch (const std::exception& exception) {
                //    spdlog::critical("IOC: {}", exception.what());
                //}
                //ioc.poll();

                // Run the I/O service. The call will return when
                // the socket is closed.
                ioc.run();
                //while (!ioc.stopped()) { ioc.poll(); }


                //std::thread([&ioc] {for (;;) { ioc.poll(); }});
                //std::thread([] { ioc.run(); });
                spdlog::info("Web Exiting...");
            }
*/

        void Test2()
        {
            try
            {
                // Check command line arguments.
                const std::string host = "ws-feed.pro.coinbase.com";
                auto const port = "443";
                auto const rpcJson = R"({
    "type": "subscribe",
    "product_ids": [
        "ETH-USD",
        "ETH-EUR"
    ],
    "channels": [
        "level2",
        "heartbeat",
        {
            "name": "ticker",
            "product_ids": [
                "ETH-BTC",
                "ETH-USD"
            ]
        }
    ]
})";

                // The io_context is required for all I/O
                net::io_context ioc;

                // The SSL context is required, and holds certificates
                ssl::context ctx{ ssl::context::tlsv12_client };
                //ssl::context ctx{ ssl::context::sslv23_client };
                

                // This holds the root certificate used for verification
                //load_root_certificates(ctx);

                // These objects perform our I/O
                tcp::resolver resolver{ ioc };
                websocket::stream<beast::ssl_stream<tcp::socket>> ws{ ioc, ctx };
              
                // Set SNI Hostname (many hosts need this to handshake successfully)
                if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str()))
                {
                    boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
                    throw boost::system::system_error{ ec };
                }
                spdlog::debug("here1");
                // Look up the domain name
                auto const results = resolver.resolve(host, port);
                spdlog::debug("here2");
                // Make the connection on the IP address we get from a lookup
                net::connect(ws.next_layer().next_layer(), results.begin(), results.end());
                spdlog::debug("here3");
                // Perform the SSL handshake
                ws.next_layer().handshake(ssl::stream_base::client);
                spdlog::debug("here4");
                // Set a decorator to change the User-Agent of the handshake
                ws.set_option(websocket::stream_base::decorator(
                    [host](websocket::request_type& req)
                    {
                        req.set(http::field::host, host);
                        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                        req.set(http::field::content_type, "application/json");
                    }));
                spdlog::debug("here5");
                // Perform the websocket handshake
                //ws.handshake(host, "/api/2/ws");
                ws.handshake(host, "/");
                spdlog::debug("here6");
                // Our message in this case should be stringified JSON-RPC request
                ws.write(net::buffer(std::string(rpcJson)));
                spdlog::debug("here7");
                // This buffer will hold the incoming message
                beast::flat_buffer buffer;

                // Read a message into our buffer
                ws.read(buffer);
                spdlog::debug("here8");

                // The make_printable() function helps print a ConstBufferSequence
                //spdlog::debug("{}", beast::make_printable(buffer.data()));
                std::cout << beast::make_printable(buffer.data()) << std::endl;

                // Close the WebSocket connection
                ws.close(websocket::close_code::normal);
                spdlog::debug("here9");
                // If we get here then the connection is closed gracefully

                
            }
            catch (const std::exception& e)
            {
                spdlog::error("Error: {}", e.what());
            }
        }
    };
}