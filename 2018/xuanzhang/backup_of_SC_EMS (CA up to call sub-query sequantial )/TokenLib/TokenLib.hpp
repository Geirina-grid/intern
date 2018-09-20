/******************************************************************************
 * Copyright (c) 2015-2016, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenLib.hpp: a system library of token conversion function declaration. 
 *
 * - It is an N-tokens-in, one-token-out function. N is one or more.
 * - All functions must use one of the following signatures, 
 *   you can have different function name but you must follow the name convetion
 *   using snake case gsql_funcname (...). E.g.  gsql_find_max_digit (...).
 *   see http://en.wikipedia.org/wiki/Snake_case
 *
 * - A token function can have nested other token function;
 *   The out-most token function should return the same type
 *   as the targeted attribute type specified in the 
 *   vertex/edge schema.
 *  
 *   1. string[] -> string 
 *
 *   The UDF token conversion functions will take N input char 
 *   array and do a customized conversion. Then, put the 
 *   converted char array to the output char buffer.
 *
 *     extern "C" void funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum, char* const oToken, uint32_t& oTokenLen);
 *     
 *      @param: iToken: 1 or more input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *      @param: iTokenNum: how many input tokens 
 *      @param: oToken: the output token buffer; caller will prepare this buffer.
 *      @param: oTokenLen: the output token length 
 *
 *      Note: extern "C" make C++ compiler not change/mangle the function name.
 *      Note: To avoid array out of boundary issue in oToken buffer, it is 
 *            recommended to add semantic check to ensure oToken length does 
 *            not exceed  OutputTokenBufferSize parameter specified in the 
 *            shell config.
 *
 *  
 *   2. string[] -> int/bool/float
 *
 *     extern "C" uint64_t funcName (const char* const iToken[], 
 *     uint32_t iTokenLen[], uint32_t iTokenNum)
 *
 *     extern "C" bool funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum)
 *
 *     extern "C" float funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum)
 *
 *      @param: iToken: 1 or more input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *      @param: iTokenNum: how many input tokens 
 *
 *   Think token function as a UDF designed to combine N specific columns into 
 *   one column before we load them into graph store.
 * 
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named Concat(), we can use it in the DDL shell as below
 *      values( $1, Concat($2,$3), $3...)
 *
 *
 * - This header file will be included in the TokenBank1.cpp
 *
 * - The implementation is in TokenLib.cpp
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: ZZ, MW
 ******************************************************************************/

#ifndef TOKENLIB_HPP_
#define TOKENLIB_HPP_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <stdbool.h>
#include <cstdlib>

using namespace std;

 /**
  * This function concatenates all input tokens into one string.
  * @par  Example 
  *       load "source_file" to vertex v values ($0, $1, gsql_concat($1, $2)); 
  */
extern "C" void gsql_concat(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char* const oToken, uint32_t& oTokenLen);


/**
 * This function will convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, gsql_reverse($2)); 
 */
extern "C"  void gsql_reverse (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char *const oToken, uint32_t& oTokenLen);

/**
 * This funcion will convert timestamp to epoch seconds. 
 *
 * It only supports 3 types of input:
 *  - "%Y-%m-%d %H:%M:%S"<br>
 *  - "%Y/%m/%d %H:%M:%S"<br>
 *  - "%Y-%m-%dT%H:%M:%S.000z"<br> 
 * e.g.<br>
 *  - "2014-4-17 12:22:23"<br>
 *  - "2014/4/17 12:22:23"<br>
 *
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, gsql_ts_to_epoch_seconds($2)); 
 */
extern "C" uint64_t gsql_ts_to_epoch_seconds(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This funcion will replace space with ((char)30).
 * @par  Example 
 *       load "source_file" to to temp_table t1(pid,pname,rating,mid) values($0, $1, flatten(gsql_split_by_space($2)));<br>
 *       load temp_table t1 to vertex v values ($pid, $pid, $pname);<br>
 *       load temp_table t1 to edge e values ($pid, $mid, $rating);
 */
extern "C" void gsql_split_by_space (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
         char *const oToken, uint32_t& oTokenLen);

/**
 * This function converts a string to boolean.
 * - If the string is "true" or "t", return TRUE.
 * - O/w return FALSE.
 * - String is compared ignoring case considerations.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1, gsql_to_bool($2));
 */
extern "C"  bool gsql_to_bool(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function converts a string to int64. Apply implicit conversion from double to int
 * Examples:
 * - "123" -> 123
 * - "-45" -> -45
 * - "24.35" -> 24
 * - ".34" -> 0
 * - "34e5" -> 3400000
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1, gsql_to_int($2));
 */
extern "C" int64_t gsql_to_int(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function converts a string to unsigned int64. Apply implicit conversion from double to int
 * Examples:
 * - "123" -> 123
 * - "24.35" -> 24
 * - ".34" -> 0
 * - "34e5" -> 3400000
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1, gsql_to_uint($2));
 */
extern "C" uint64_t gsql_to_uint(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function returns the currect epoch time.
 *
 * @par Example
 *       load "source_file" to vertex v values ($0, $1, gsql_current_time_epoch(0));
 */
extern "C"  uint64_t gsql_current_time_epoch (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function checks whether the input string is "true" or "t" (case insensitive),
 * returns true if it is "true" or "t".
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_true($2);
 */
extern "C"  bool gsql_is_true(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function checks whether the input string is "false" or "f" (case insensitive),
 * and returns true if it is "false" or "f".
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_false($2);
 */
extern "C"  bool gsql_is_false(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This function checks whether the input string is empty 
 * after removing white space.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_not_empty_string($2);
 */
extern "C"  bool gsql_is_not_empty_string(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This funtion compares two string (case sensitive), and returns true if they are equal.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_token_equal($1, "test");
 */

extern "C" bool gsql_token_equal(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This funtion compares two string (case insensitive), and returns true if they are equal.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_token_ignore_case_equal($1, "Test");
 */

extern "C" bool gsql_token_ignore_case_equal(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * Predicate testing helper function.
 *    E.g. WHERE $1 IS NUMERIC
 *    Will call this function.
 *
 * IsNumeric test whether a string is a numeric number.
 *
 * Numeric number is defined as follows in regular expression:
 *      (+/-)?[0-9][0-9]*(.[0-9])?[0-9]*
 *
 * Any space appear in between will not be taken as numeric number.
 * Space in front and at the end is OK.
 */

bool IsNumeric (std::string s);

#endif /* TOKENLIB_HPP_ */
