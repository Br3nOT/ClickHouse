#!/usr/bin/env bash

CURDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=../shell_config.sh
. "$CURDIR"/../shell_config.sh


for i in $(seq 1 10);
do
    $CLICKHOUSE_CLIENT --query_id="02366_$i" -q "insert into function s3('http://localhost:11111/test/02367_data_$i.jsonl') select range(number % 1000) from numbers(100000) settings s3_truncate_on_insert=1, output_format_parallel_formatting=1" 2> /dev/null &
done

sleep 2

$CLICKHOUSE_CLIENT -q "kill query where startsWith(query_id, '02367_') sync" > /dev/null

for i in $(seq 1 10);
do
    $CLICKHOUSE_CLIENT --query_id="02367_$i" -q "insert into function s3('http://localhost:11111/test/02366_data_$i.jsonl') select range(number % 1000) from numbers(100000) settings s3_truncate_on_inser=1, output_format_parallel_formatting=0" 2> /dev/null &
done

sleep 2

$CLICKHOUSE_CLIENT -q "kill query where startsWith(query_id, '02367_') sync" > /dev/null
