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
    best_ask numeric NOT NULL
);


ALTER TABLE crypto.ticker OWNER TO crypto;

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

