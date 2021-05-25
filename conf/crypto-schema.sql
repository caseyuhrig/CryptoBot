--
-- PostgreSQL database dump
--

-- Dumped from database version 12.2
-- Dumped by pg_dump version 12.2

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: crypto; Type: DATABASE; Schema: -; Owner: crypto
--

CREATE DATABASE crypto WITH TEMPLATE = template0 ENCODING = 'UTF8' LC_COLLATE = 'English_United States.1252' LC_CTYPE = 'English_United States.1252' TABLESPACE = crypto;


ALTER DATABASE crypto OWNER TO crypto;

\connect crypto

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: crypto; Type: SCHEMA; Schema: -; Owner: crypto
--

CREATE SCHEMA crypto;


ALTER SCHEMA crypto OWNER TO crypto;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: ticker; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.ticker (
    id bigint NOT NULL,
    exchange_id bigint NOT NULL,
    trading_pair_id bigint NOT NULL,
    sequence bigint NOT NULL,
    price numeric NOT NULL,
    price_time timestamp with time zone NOT NULL,
    exchange_trade_id bigint NOT NULL,
    last_size numeric NOT NULL,
    side integer NOT NULL,
    open_24h numeric NOT NULL,
    volume_24h numeric NOT NULL,
    low_24h numeric NOT NULL,
    high_24h numeric NOT NULL,
    volume_30d numeric NOT NULL,
    best_bid numeric NOT NULL,
    best_ask numeric NOT NULL,
    price_epoch numeric
);


ALTER TABLE crypto.ticker OWNER TO crypto;

--
-- Name: trading_pair; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.trading_pair (
    id bigint NOT NULL,
    identifier text NOT NULL,
    base_currency text NOT NULL,
    quote_currency text NOT NULL
);


ALTER TABLE crypto.trading_pair OWNER TO crypto;

--
-- Name: view_ticker_x; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_ticker_x AS
 SELECT trading_pair.identifier,
    ticker.id,
    ticker.exchange_id,
    ticker.trading_pair_id,
    ticker.sequence,
    ticker.price,
    ticker.price_time,
    ticker.exchange_trade_id,
    ticker.last_size,
    ticker.side,
    ticker.open_24h,
    ticker.volume_24h,
    ticker.low_24h,
    ticker.high_24h,
    ticker.volume_30d,
    ticker.best_bid,
    ticker.best_ask,
    date_trunc('hour'::text, ticker.price_time) AS price_hour
   FROM crypto.ticker,
    crypto.trading_pair
  WHERE (ticker.trading_pair_id = trading_pair.id);


ALTER TABLE crypto.view_ticker_x OWNER TO crypto;

--
-- Name: chart_candlestick_hourly; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.chart_candlestick_hourly AS
 WITH intervals AS (
         SELECT start.start,
            (start.start + '01:00:00'::interval) AS "end"
           FROM generate_series(date_trunc('hour'::text, (now() - '2 days'::interval)), date_trunc('hour'::text, now()), '01:00:00'::interval) start(start)
        )
 SELECT DISTINCT intervals.start AS price_hour,
    date_part('epoch'::text, intervals.start) AS hour_epoch,
    min(mb.price) OVER w AS low,
    max(mb.price) OVER w AS high,
    first_value(mb.price) OVER w AS open,
    last_value(mb.price) OVER w AS close,
        CASE
            WHEN (first_value(mb.price) OVER w > last_value(mb.price) OVER w) THEN '-1'::integer
            ELSE 1
        END AS market
   FROM (intervals
     JOIN crypto.view_ticker_x mb ON (((mb.identifier = 'ADA-USD'::text) AND (mb.price_time >= intervals.start) AND (mb.price_time < intervals."end"))))
  WINDOW w AS (PARTITION BY intervals.start ORDER BY mb.price_time ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
  ORDER BY intervals.start;


ALTER TABLE crypto.chart_candlestick_hourly OWNER TO crypto;

--
-- Name: chart_candlestick_hourly_limits; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.chart_candlestick_hourly_limits AS
 SELECT min(chart_candlestick_hourly.hour_epoch) AS min_hour_epoch,
    max(chart_candlestick_hourly.hour_epoch) AS max_hour_epoch,
    LEAST(min(chart_candlestick_hourly.low), min(chart_candlestick_hourly.high), min(chart_candlestick_hourly.open), min(chart_candlestick_hourly.close)) AS min_price,
    GREATEST(max(chart_candlestick_hourly.low), max(chart_candlestick_hourly.high), max(chart_candlestick_hourly.open), max(chart_candlestick_hourly.close)) AS max_price
   FROM crypto.chart_candlestick_hourly;


ALTER TABLE crypto.chart_candlestick_hourly_limits OWNER TO crypto;

--
-- Name: chart_candlestick_hourly_limits_m; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.chart_candlestick_hourly_limits_m (
    min_hour_epoch double precision,
    max_hour_epoch double precision,
    min_price numeric,
    max_price numeric
);


ALTER TABLE crypto.chart_candlestick_hourly_limits_m OWNER TO crypto;

--
-- Name: chart_candlestick_hourly_m; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.chart_candlestick_hourly_m (
    price_hour timestamp with time zone,
    hour_epoch double precision,
    low numeric,
    high numeric,
    open numeric,
    close numeric,
    market integer
);


ALTER TABLE crypto.chart_candlestick_hourly_m OWNER TO crypto;

--
-- Name: crypto_currency; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.crypto_currency (
    id bigint NOT NULL,
    identifier text NOT NULL
);


ALTER TABLE crypto.crypto_currency OWNER TO crypto;

--
-- Name: crypto_currency_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.crypto_currency_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.crypto_currency_id_seq OWNER TO crypto;

--
-- Name: crypto_currency_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.crypto_currency_id_seq OWNED BY crypto.crypto_currency.id;


--
-- Name: exchange; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.exchange (
    id bigint NOT NULL,
    identifier text NOT NULL
);


ALTER TABLE crypto.exchange OWNER TO crypto;

--
-- Name: exchange_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.exchange_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.exchange_id_seq OWNER TO crypto;

--
-- Name: exchange_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.exchange_id_seq OWNED BY crypto.exchange.id;


--
-- Name: fiat_currency; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.fiat_currency (
    id bigint NOT NULL,
    identifier text NOT NULL
);


ALTER TABLE crypto.fiat_currency OWNER TO crypto;

--
-- Name: fiat_currency_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.fiat_currency_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.fiat_currency_id_seq OWNER TO crypto;

--
-- Name: fiat_currency_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.fiat_currency_id_seq OWNED BY crypto.fiat_currency.id;


--
-- Name: fills; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.fills (
    id bigint NOT NULL,
    exchange_id bigint NOT NULL,
    trading_pair_id bigint NOT NULL,
    price numeric NOT NULL,
    fill_size numeric NOT NULL,
    order_id text NOT NULL,
    created_at timestamp with time zone,
    liquidity text,
    fee numeric,
    settled boolean NOT NULL,
    side_id integer NOT NULL
);


ALTER TABLE crypto.fills OWNER TO crypto;

--
-- Name: fills_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.fills_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.fills_id_seq OWNER TO crypto;

--
-- Name: fills_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.fills_id_seq OWNED BY crypto.fills.id;


--
-- Name: orders; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.orders (
    id bigint NOT NULL,
    exchange text NOT NULL,
    order_id text NOT NULL,
    product_id text NOT NULL,
    price numeric,
    order_size numeric,
    order_type text NOT NULL,
    side text NOT NULL,
    stp text,
    time_in_force text,
    post_only boolean,
    created_at timestamp with time zone NOT NULL,
    done_at timestamp with time zone,
    done_reason text,
    funds numeric,
    specified_funds numeric,
    fill_fees numeric,
    filled_size numeric,
    executed_value numeric,
    status text,
    settled boolean
);


ALTER TABLE crypto.orders OWNER TO crypto;

--
-- Name: orders_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.orders_id_seq OWNER TO crypto;

--
-- Name: orders_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.orders_id_seq OWNED BY crypto.orders.id;


--
-- Name: side; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.side (
    id integer NOT NULL,
    identifier text NOT NULL
);


ALTER TABLE crypto.side OWNER TO crypto;

--
-- Name: ticker_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.ticker_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.ticker_id_seq OWNER TO crypto;

--
-- Name: ticker_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.ticker_id_seq OWNED BY crypto.ticker.id;


--
-- Name: trading_account; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.trading_account (
    id bigint NOT NULL,
    exchange text NOT NULL,
    account_id text NOT NULL,
    profile_id text NOT NULL,
    currency text NOT NULL,
    balance numeric DEFAULT 0.0 NOT NULL,
    available numeric DEFAULT 0.0 NOT NULL,
    hold numeric DEFAULT 0.0 NOT NULL,
    trading_enabled boolean DEFAULT false NOT NULL
);


ALTER TABLE crypto.trading_account OWNER TO crypto;

--
-- Name: trading_account_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.trading_account_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.trading_account_id_seq OWNER TO crypto;

--
-- Name: trading_account_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.trading_account_id_seq OWNED BY crypto.trading_account.id;


--
-- Name: trading_pair_id_seq; Type: SEQUENCE; Schema: crypto; Owner: crypto
--

CREATE SEQUENCE crypto.trading_pair_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE crypto.trading_pair_id_seq OWNER TO crypto;

--
-- Name: trading_pair_id_seq; Type: SEQUENCE OWNED BY; Schema: crypto; Owner: crypto
--

ALTER SEQUENCE crypto.trading_pair_id_seq OWNED BY crypto.trading_pair.id;


--
-- Name: view_current; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_current AS
 SELECT a.order_id,
    a.product_id,
    a.price,
    a.cur_price,
    a.order_size,
    a.amount,
    a.side,
    a.cur_amount,
    a.created_at,
    a.status,
    a.fill_fees,
    a.settled,
    (a.cur_amount - a.amount) AS diff,
    ((a.cur_amount - a.amount) - a.fill_fees) AS diff_fees
   FROM ( SELECT orders.order_id,
            orders.product_id,
            orders.price,
            ( SELECT ticker.price
                   FROM crypto.ticker
                  WHERE (ticker.trading_pair_id = trading_pair.id)
                  ORDER BY ticker.price_time DESC
                 LIMIT 1) AS cur_price,
            orders.order_size,
            (orders.price * orders.order_size) AS amount,
            orders.side,
            (( SELECT ticker.price
                   FROM crypto.ticker
                  WHERE (ticker.trading_pair_id = trading_pair.id)
                  ORDER BY ticker.price_time DESC
                 LIMIT 1) * orders.order_size) AS cur_amount,
            orders.created_at,
            orders.status,
            orders.fill_fees,
            orders.settled
           FROM crypto.orders,
            crypto.trading_pair
          WHERE ((orders.side = 'buy'::text) AND (orders.product_id = trading_pair.identifier))) a;


ALTER TABLE crypto.view_current OWNER TO crypto;

--
-- Name: view_graph; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_graph AS
 SELECT date_part('epoch'::text, ticker.price_time) AS price_epoch,
    ticker.price_time,
    trading_pair.identifier,
    ticker.price,
    ticker.low_24h,
    ticker.high_24h
   FROM crypto.ticker,
    crypto.trading_pair
  WHERE ((ticker.trading_pair_id = trading_pair.id) AND (trading_pair.identifier = 'ADA-USD'::text) AND (ticker.price_time > (now() - '2 days'::interval)))
  ORDER BY ticker.price_time;


ALTER TABLE crypto.view_graph OWNER TO crypto;

--
-- Name: view_graph_limits; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_graph_limits AS
 SELECT min(view_graph.price_epoch) AS min_price_epoch,
    max(view_graph.price_epoch) AS max_price_epoch,
    min(view_graph.price) AS min_price,
    max(view_graph.price) AS max_price
   FROM crypto.view_graph;


ALTER TABLE crypto.view_graph_limits OWNER TO crypto;

--
-- Name: view_graph_limits_m; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.view_graph_limits_m (
    min_price_epoch double precision,
    max_price_epoch double precision,
    min_price numeric,
    max_price numeric
);


ALTER TABLE crypto.view_graph_limits_m OWNER TO crypto;

--
-- Name: view_graph_m; Type: TABLE; Schema: crypto; Owner: crypto
--

CREATE TABLE crypto.view_graph_m (
    price_epoch double precision,
    price_time timestamp with time zone,
    identifier text,
    price numeric,
    low_24h numeric,
    high_24h numeric
);


ALTER TABLE crypto.view_graph_m OWNER TO crypto;

--
-- Name: view_hourly_price; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_hourly_price AS
 SELECT b.identifier,
    avg(a.price) AS price_avg,
    max(a.price) AS price_max,
    min(a.price) AS price_min,
    (max(a.price) - min(a.price)) AS price_diff,
    date_trunc('hour'::text, a.price_time) AS price_hour,
    count(*) AS ticks
   FROM crypto.ticker a,
    crypto.trading_pair b
  WHERE ((a.trading_pair_id = b.id) AND (b.identifier = 'ADA-USD'::text) AND (a.price_time > (now() - '2 days'::interval)))
  GROUP BY b.identifier, (date_trunc('hour'::text, a.price_time))
  ORDER BY (date_trunc('hour'::text, a.price_time));


ALTER TABLE crypto.view_hourly_price OWNER TO crypto;

--
-- Name: view_ordered; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_ordered AS
 WITH ordered AS (
         SELECT row_number() OVER (ORDER BY ticker.price_time DESC) AS n,
            ticker.id,
            ticker.exchange_id,
            ticker.trading_pair_id,
            ticker.sequence,
            ticker.price,
            ticker.price_time,
            ticker.exchange_trade_id,
            ticker.last_size,
            ticker.side,
            ticker.open_24h,
            ticker.volume_24h,
            ticker.low_24h,
            ticker.high_24h,
            ticker.volume_30d,
            ticker.best_bid,
            ticker.best_ask
           FROM crypto.ticker,
            crypto.trading_pair
          WHERE ((ticker.trading_pair_id = trading_pair.id) AND (trading_pair.identifier = 'ADA-USD'::text))
        )
 SELECT a.n AS a_n,
    b.n AS b_n,
    a.id AS a_id,
    b.id AS b_id,
    a.price AS a_price,
    b.price AS b_price,
    a.price_time AS a_price_time,
    b.price_time AS b_price_time,
    (b.price_time - a.price_time) AS diff
   FROM (ordered b
     JOIN ordered a ON (((a.n + 1) = b.n)));


ALTER TABLE crypto.view_ordered OWNER TO crypto;

--
-- Name: view_ticker; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_ticker AS
 SELECT ticker.id,
    ticker.sequence,
    ((trading_pair.base_currency || '-'::text) || trading_pair.quote_currency) AS product_id,
    trading_pair.base_currency,
    trading_pair.quote_currency,
    side.identifier AS side,
    ticker.price,
    ticker.price_time
   FROM crypto.ticker,
    crypto.trading_pair,
    crypto.side
  WHERE ((ticker.trading_pair_id = trading_pair.id) AND (side.id = ticker.side))
  ORDER BY ticker.price_time DESC;


ALTER TABLE crypto.view_ticker OWNER TO crypto;

--
-- Name: view_ticker_ada_stream; Type: VIEW; Schema: crypto; Owner: crypto
--

CREATE VIEW crypto.view_ticker_ada_stream AS
 SELECT view_ticker.id,
    view_ticker.sequence,
    view_ticker.product_id,
    view_ticker.base_currency,
    view_ticker.quote_currency,
    view_ticker.side,
    view_ticker.price,
    view_ticker.price_time
   FROM crypto.view_ticker
  WHERE ((view_ticker.product_id = 'ADA-USD'::text) AND (view_ticker.price_time > (now() - '00:00:01'::interval)))
 LIMIT 10;


ALTER TABLE crypto.view_ticker_ada_stream OWNER TO crypto;

--
-- Name: crypto_currency id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.crypto_currency ALTER COLUMN id SET DEFAULT nextval('crypto.crypto_currency_id_seq'::regclass);


--
-- Name: exchange id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.exchange ALTER COLUMN id SET DEFAULT nextval('crypto.exchange_id_seq'::regclass);


--
-- Name: fiat_currency id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fiat_currency ALTER COLUMN id SET DEFAULT nextval('crypto.fiat_currency_id_seq'::regclass);


--
-- Name: fills id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fills ALTER COLUMN id SET DEFAULT nextval('crypto.fills_id_seq'::regclass);


--
-- Name: orders id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.orders ALTER COLUMN id SET DEFAULT nextval('crypto.orders_id_seq'::regclass);


--
-- Name: ticker id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.ticker ALTER COLUMN id SET DEFAULT nextval('crypto.ticker_id_seq'::regclass);


--
-- Name: trading_account id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_account ALTER COLUMN id SET DEFAULT nextval('crypto.trading_account_id_seq'::regclass);


--
-- Name: trading_pair id; Type: DEFAULT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_pair ALTER COLUMN id SET DEFAULT nextval('crypto.trading_pair_id_seq'::regclass);


--
-- Name: crypto_currency crypto_currency_identifier_key; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.crypto_currency
    ADD CONSTRAINT crypto_currency_identifier_key UNIQUE (identifier);


--
-- Name: crypto_currency crypto_currency_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.crypto_currency
    ADD CONSTRAINT crypto_currency_pkey PRIMARY KEY (id);


--
-- Name: exchange exchange_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.exchange
    ADD CONSTRAINT exchange_pkey PRIMARY KEY (id);


--
-- Name: fiat_currency fiat_currency_identifier_key; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fiat_currency
    ADD CONSTRAINT fiat_currency_identifier_key UNIQUE (identifier);


--
-- Name: fiat_currency fiat_currency_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fiat_currency
    ADD CONSTRAINT fiat_currency_pkey PRIMARY KEY (id);


--
-- Name: fills fills_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fills
    ADD CONSTRAINT fills_pkey PRIMARY KEY (id);


--
-- Name: orders orders_exchange_order_id_key; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.orders
    ADD CONSTRAINT orders_exchange_order_id_key UNIQUE (exchange, order_id);


--
-- Name: orders orders_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.orders
    ADD CONSTRAINT orders_pkey PRIMARY KEY (id);


--
-- Name: side side_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.side
    ADD CONSTRAINT side_pkey PRIMARY KEY (id);


--
-- Name: ticker ticker_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.ticker
    ADD CONSTRAINT ticker_pkey PRIMARY KEY (id);


--
-- Name: trading_account trading_account_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_account
    ADD CONSTRAINT trading_account_pkey PRIMARY KEY (id);


--
-- Name: trading_pair trading_pair_base_currency_quote_currency_key; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_pair
    ADD CONSTRAINT trading_pair_base_currency_quote_currency_key UNIQUE (base_currency, quote_currency);


--
-- Name: trading_pair trading_pair_identifier_key; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_pair
    ADD CONSTRAINT trading_pair_identifier_key UNIQUE (identifier);


--
-- Name: trading_pair trading_pair_pkey; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_pair
    ADD CONSTRAINT trading_pair_pkey PRIMARY KEY (id);


--
-- Name: exchange unique_id; Type: CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.exchange
    ADD CONSTRAINT unique_id UNIQUE (identifier);


--
-- Name: idx_ticker_price_time; Type: INDEX; Schema: crypto; Owner: crypto
--

CREATE INDEX idx_ticker_price_time ON crypto.ticker USING btree (price_time);


--
-- Name: ticker_price_idx; Type: INDEX; Schema: crypto; Owner: crypto
--

CREATE INDEX ticker_price_idx ON crypto.ticker USING btree (price);


--
-- Name: ticker_trading_pair_id_idx; Type: INDEX; Schema: crypto; Owner: crypto
--

CREATE INDEX ticker_trading_pair_id_idx ON crypto.ticker USING btree (trading_pair_id);


--
-- Name: trading_pair_identifier_idx; Type: INDEX; Schema: crypto; Owner: crypto
--

CREATE INDEX trading_pair_identifier_idx ON crypto.trading_pair USING btree (identifier);


--
-- Name: fills fills_exchange_id_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fills
    ADD CONSTRAINT fills_exchange_id_fkey FOREIGN KEY (exchange_id) REFERENCES crypto.exchange(id);


--
-- Name: fills fills_side_id_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fills
    ADD CONSTRAINT fills_side_id_fkey FOREIGN KEY (side_id) REFERENCES crypto.side(id);


--
-- Name: fills fills_trading_pair_id_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.fills
    ADD CONSTRAINT fills_trading_pair_id_fkey FOREIGN KEY (trading_pair_id) REFERENCES crypto.trading_pair(id);


--
-- Name: orders orders_exchange_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.orders
    ADD CONSTRAINT orders_exchange_fkey FOREIGN KEY (exchange) REFERENCES crypto.exchange(identifier);


--
-- Name: ticker ticker_exchange_id_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.ticker
    ADD CONSTRAINT ticker_exchange_id_fkey FOREIGN KEY (exchange_id) REFERENCES crypto.exchange(id);


--
-- Name: ticker ticker_side_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.ticker
    ADD CONSTRAINT ticker_side_fkey FOREIGN KEY (side) REFERENCES crypto.side(id);


--
-- Name: ticker ticker_trading_pair_id_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.ticker
    ADD CONSTRAINT ticker_trading_pair_id_fkey FOREIGN KEY (trading_pair_id) REFERENCES crypto.trading_pair(id);


--
-- Name: trading_account trading_account_exchange_fkey; Type: FK CONSTRAINT; Schema: crypto; Owner: crypto
--

ALTER TABLE ONLY crypto.trading_account
    ADD CONSTRAINT trading_account_exchange_fkey FOREIGN KEY (exchange) REFERENCES crypto.exchange(identifier);


--
-- PostgreSQL database dump complete
--

