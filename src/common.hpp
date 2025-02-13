/*-------------------------------------------------------------------------
 *
 * common.hpp
 *		  FDW routines for parquet_s3_fdw
 *
 * Portions Copyright (c) 2021, TOSHIBA CORPORATION
 * Portions Copyright (c) 2018-2019, adjust GmbH
 *
 * IDENTIFICATION
 *		  contrib/parquet_s3_fdw/src/common.hpp
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARQUET_FDW_COMMON_HPP
#define PARQUET_FDW_COMMON_HPP

#include <cstdarg>
#include <cstddef>

#include "arrow/api.h"

extern "C"
{
#include "postgres.h"
#include "utils/jsonb.h"
}

#define ERROR_STR_LEN 512

#if PG_VERSION_NUM < 110000
#define DatumGetJsonbP DatumGetJsonb
#define JsonbPGetDatum JsonbGetDatum
#endif

#define to_postgres_timestamp(tstype, i, ts)                    \
    switch ((tstype)->unit()) {                                 \
        case arrow::TimeUnit::SECOND:                           \
            ts = time_t_to_timestamptz((i)); break;             \
        case arrow::TimeUnit::MILLI:                            \
            ts = time_t_to_timestamptz((i) / 1000); break;      \
        case arrow::TimeUnit::MICRO:                            \
            ts = time_t_to_timestamptz((i) / 1000000); break;   \
        case arrow::TimeUnit::NANO:                             \
            ts = time_t_to_timestamptz((i) / 1000000000); break;\
        default:                                                \
            elog(ERROR, "parquet_s3_fdw: Timestamp of unknown precision: %d",   \
                 (tstype)->unit());                             \
    }


struct Error : std::exception
{
    char text[ERROR_STR_LEN];

    Error(char const* fmt, ...) __attribute__((format(printf,2,3))) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(text, sizeof text, fmt, ap);
        va_end(ap);
    }

    char const* what() const throw() { return text; }
};


void *exc_palloc(std::size_t size);
Oid to_postgres_type(int arrow_type);
Datum bytes_to_postgres_type(const char *bytes, Size len, const arrow::DataType *arrow_type);
char *tolowercase(const char *input, char *output);
arrow::Type::type get_arrow_list_elem_type(arrow::DataType *type);
void datum_to_jsonb(Datum value, Oid typoid, bool isnull, FmgrInfo *outfunc,
                    JsonbParseState *result, JsonbIteratorToken seq);
void push_jsonb_string_key(JsonbParseState *parseState, char *key_name);

#endif
