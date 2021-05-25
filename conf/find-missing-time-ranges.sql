SELECT
    a.n AS a_n,
    b.n AS b_n,
    a.id AS a_id,
    b.id AS b_id,
    a.price AS a_price,
    b.price AS b_price,
    a.price_time AS a_price_time,
    b.price_time AS b_price_time,
    b.price_time - a.price_time AS diff
FROM
(SELECT
    ROW_NUMBER() OVER (ORDER BY price_time DESC) n,
    ticker.*
FROM
    ticker, trading_pair
WHERE
    ticker.trading_pair_id = trading_pair.id AND
    trading_pair.identifier = 'ADA-USD'
) AS a,
(SELECT
    ROW_NUMBER() OVER (ORDER BY price_time DESC) n,
    ticker.*
FROM
    ticker, trading_pair
WHERE
    ticker.trading_pair_id = trading_pair.id AND
    trading_pair.identifier = 'ADA-USD'
) AS b
WHERE
    b.n = a.n + 1 AND
    a.n <> b.n
ORDER BY
    a.n
LIMIT 10
;

 AND
    b.price_time - a.price_time > '1 minutes'::INTERVAL
LIMIT 10
;

DROP VIEW view_ordered;
CREATE VIEW view_ordered AS
WITH ordered AS
(
    SELECT
        ROW_NUMBER() OVER (ORDER BY price_time DESC) n,
        ticker.*
    FROM
        ticker, trading_pair
    WHERE
        ticker.trading_pair_id = trading_pair.id AND
        trading_pair.identifier = 'ADA-USD'
)
SELECT
    a.n AS a_n,
    b.n AS b_n,
    a.id AS a_id,
    b.id AS b_id,
    a.price AS a_price,
    b.price AS b_price,
    a.price_time AS a_price_time,
    b.price_time AS b_price_time,
    b.price_time - a.price_time AS diff
  FROM ordered b JOIN ordered a
    ON a.n + 1 = b.n
;

select * from view_ordered;

SELECT
    a.n AS a_n,
    b.n AS b_n,
    a.id AS a_id,
    b.id AS b_id,
    a.price AS a_price,
    b.price AS b_price,
    a.price_time AS a_price_time,
    b.price_time AS b_price_time,
    b.price_time - a.price_time AS diff
FROM
(SELECT
    ROW_NUMBER() OVER (ORDER BY price_time DESC) n,
    ticker.*
FROM
    ticker, trading_pair
WHERE
    ticker.trading_pair_id = trading_pair.id AND
    trading_pair.identifier = 'ADA-USD'
) AS a,
(SELECT
    ROW_NUMBER() OVER (ORDER BY price_time DESC) n,
    ticker.*
FROM
    ticker, trading_pair
WHERE
    ticker.trading_pair_id = trading_pair.id AND
    trading_pair.identifier = 'ADA-USD'
) AS b
WHERE
    b.n = a.n + 1 AND
    a.n <> b.n
ORDER BY
    a.n
LIMIT 10
;

select price, price_time, extract('epoch' from price_time) as price_epoch from ticker where age(price_time) < '2 days' order by price_time ASC

DROP VIEW view_graph_limits;
DROP VIEW view_graph;

CREATE VIEW view_graph AS
    SELECT
--        ROW_NUMBER() OVER (ORDER BY price_time ASC)::numeric/1000::numeric n,
        extract(epoch from ticker.price_time) AS price_epoch,
        ticker.price_time,
        trading_pair.identifier,
        ticker.price,
        ticker.low_24h,
        ticker.high_24h
    FROM
        ticker, trading_pair
    WHERE
        ticker.trading_pair_id = trading_pair.id
        AND trading_pair.identifier = 'ADA-USD'
        AND ticker.price_time > NOW() - '2 days'::INTERVAL
    ORDER BY
        ticker.price_time ASC
;

CREATE VIEW view_graph_limits AS
    SELECT
        --min(n) AS min_n,
        --max(n) AS max_n,
        min(price_epoch) AS min_price_epoch,
        max(price_epoch) AS max_price_epoch,
        min(price) AS min_price,
        max(price) AS max_price
FROM view_graph;

SELECT * FROM view_graph LIMIT 10;
SELECT * FROM view_graph_limits;


DROP VIEW view_hourly_price2;
DROP VIEW view_hourly_price;
CREATE VIEW view_hourly_price AS
SELECT
  b.identifier AS identifier,
  AVG(a.price) AS price_avg,
  MAX(a.price) AS price_max,
  MIN(a.price) AS price_min,
  MAX(a.price) - MIN(a.price) AS price_diff,
  date_trunc('hour', a.price_time) AS price_hour,
  count(*) AS ticks
FROM
  ticker AS a,
  trading_pair AS b
WHERE
  a.trading_pair_id = b.id
  AND b.identifier = 'ADA-USD'
  AND a.price_time > NOW() - '2 days'::INTERVAL
GROUP BY
  b.identifier,
  date_trunc('hour', a.price_time)
ORDER BY
  date_trunc('hour', a.price_time) ASC
;

SELECT * FROM view_hourly_price LIMIT 10;



select
  view_hourly_price.*,
  (
    SELECT price FROM view_ticker_x where
    view_ticker_x.price_hour = view_hourly_price.price_hour
    AND view_ticker_x.identifier = view_hourly_price.identifier
    ORDER BY
      view_ticker_x.price_time ASC
    LIMIT 1
  ) as open_price
from
  view_hourly_price
limit 10
;

/*****************************
*/

DROP VIEW chart_candlestick_hourly_limits;
DROP VIEW chart_candlestick_hourly;

CREATE VIEW chart_candlestick_hourly AS
with intervals as (
select start, start + interval '1hour' as end
from generate_series(
  date_trunc('hour', NOW() - '2 days'::interval) --(SELECT date_trunc('hour', MIN(price_time)) FROM ticker)
  ,
  date_trunc('hour', NOW()) --(SELECT date_trunc('hour', MAX(price_time)) FROM ticker)
  , interval '1hour') as start)
select distinct
  intervals.start as price_hour,
  extract(epoch from intervals.start) as hour_epoch,
  min(price) over w as low,
  max(price) over w as high,
  first_value(price) over w as open,
  last_value(price) over w as close,
  case when first_value(price) over w > last_value(price) over w then -1 else 1 end as market
from
  intervals
  join ticker mb on
    mb.trading_pair_id = 133 and
    mb.price_time >= intervals.start and
    mb.price_time < intervals.end
window w as (partition by intervals.start order by mb.price_time asc rows between unbounded preceding and unbounded following)
order by intervals.start
;

CREATE VIEW chart_candlestick_hourly_limits AS
  SELECT
    MIN(hour_epoch) AS min_hour_epoch,
    MAX(hour_epoch) AS max_hour_epoch,
    LEAST(MIN(low),MIN(high),MIN(open),MIN(close)) AS min_price,
    GREATEST(MAX(low),MAX(high),MAX(open),MAX(close)) AS max_price
  FROM
    chart_candlestick_hourly
;


DROP TABLE chart_candlestick_hourly_m;
DROP TABLE chart_candlestick_hourly_limits_m;

CREATE TABLE chart_candlestick_hourly_m AS SELECT * FROM chart_candlestick_hourly;
CREATE TABLE chart_candlestick_hourly_limits_m AS SELECT * FROM chart_candlestick_hourly_limits;





DROP VIEW view_candle_chart;
CREATE VIEW view_candle_chart AS
with intervals as (
select start, start + interval '1hour' as end
from generate_series(
  date_trunc('hour', NOW() - '2 days'::interval) --(SELECT date_trunc('hour', MIN(price_time)) FROM ticker)
  ,
  date_trunc('hour', NOW()) --(SELECT date_trunc('hour', MAX(price_time)) FROM ticker)
  , interval '1hour') as start)
select distinct
  intervals.start as price_hour,
  min(price) over w as low,
  max(price) over w as high,
  first_value(price) over w as open,
  last_value(price) over w as close
from
  intervals
  join ticker mb on
    --mb.identifier = 'ADA-USD' and
    mb.price_time >= intervals.start and
    mb.price_time < intervals.end
window w as (partition by intervals.start order by mb.price_time asc rows between unbounded preceding and unbounded following)
    AND trading_pair tp on mb.trading_pair_id = tp.id

order by intervals.start
;

















with intervals as (
select start, start + interval '1hour' as end
from generate_series('2021-05-15 12:00', '2021-05-16 12:00', interval '1hour') as start)
select distinct
  intervals.start as price_hour,
  min(price) over w as low,
  max(price) over w as high,
  first_value(price) over w as open,
  last_value(price) over w as close
from
  intervals
  join view_ticker_x mb on
    mb.identifier = 'ADA-USD' and
    mb.price_time >= intervals.start and
    mb.price_time < intervals.end
window w as (partition by intervals.start order by mb.price_time asc rows between unbounded preceding and unbounded following)
order by intervals.start
;











DROP VIEW view_ticker_x;
CREATE VIEW view_ticker_x AS
select
  trading_pair.identifier,
  ticker.*,
  date_trunc('hour', ticker.price_time) as price_hour
from
  ticker,
  trading_pair
WHERE
  ticker.trading_pair_id = trading_pair.id
;

SELECT * FROM view_ticker_x LIMIT 2;





CREATE VIEW view_hourly_price2 AS

select
  date_trunc('hour', ticker.price_time) as price_hour
from
  ticker, trading_pair



SELECT
  c.price_time as open_time,
  c.price as open_price
FROM
  view_ticker AS c,
  trading_pair AS d
WHERE
  c.trading_pair_id = d.id
  AND d.identifier = 'ADA-USD'
ORDER BY
  c.price_time ASC
LIMIT 1
;

SELECT
  c.price_time as close_time,
  c.price as close_price
FROM
  ticker AS c,
  trading_pair AS d
WHERE
  c.trading_pair_id = d.id
  AND d.identifier = 'ADA-USD'
ORDER BY
  c.price_time DESC
LIMIT 1
;

SELECT * FROM view_hourly_price2;


SELECT
  first_value (price) OVER w AS open,
  MAX(price) OVER w AS high,
  MIN(price) OVER w as low,
  last_value(price) OVER w as close,
  date_trunc('hour', price_time)
FROM
  ticker WINDOW w AS (PARTITION BY date_trunc('hour', price_time) ORDER BY price_time)
LIMIT 2
;

SELECT
  first_value (price) OVER w AS open,
  MAX(price) OVER w AS high,
  MIN(price) OVER w as low,
  last_value(price) OVER w as close,
  date_trunc('hour', price_time)
FROM
  ticker,
  trading_pair
WHERE
  ticker.trading_pair_id = trading_pair.id
  AND trading_pair.identifier = 'ADA-USD'
  AND ticker.price_time > NOW() - '2 days'::INTERVAL
WINDOW w AS (PARTITION BY date_trunc('hour', price_time) ORDER BY price_time ASC)
LIMIT 20
;






















end
