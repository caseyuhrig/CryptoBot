

CREATE OR REPLACE VIEW view_ticker AS
SELECT
    ticker.id,
    ticker.sequence,
    trading_pair.base_currency || '-' || trading_pair.quote_currency AS product_id,
    trading_pair.base_currency AS base_currency,
    trading_pair.quote_currency AS quote_currency,
    side.identifier AS side,
    ticker.price,
    ticker.price_time
FROM
    ticker, trading_pair, side
WHERE
    ticker.trading_pair_id = trading_pair.id AND
    side.id = ticker.side
ORDER BY
    ticker.price_time DESC
;


