/*-------------------------------------------------------------------------
 *
 * pg_helper.hpp
 *		  Postgres helper for FDW compatibility with PG 15 (polyfills)
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARQUET_FDW_PG_HELPER_HPP
#define PARQUET_FDW_PG_HELPER_HPP

extern "C" {
#include "postgres.h"
#include "nodes/parsenodes.h"
}

typedef struct Value {
  NodeTag type; /* tag appropriately (eg. T_String) */
  union ValUnion {
    int ival;  /* machine integer */
    char *str; /* string */
  } val;
} Value;

int32 pg_atoi_polyfill(const char *s, int size, int c);

#endif
