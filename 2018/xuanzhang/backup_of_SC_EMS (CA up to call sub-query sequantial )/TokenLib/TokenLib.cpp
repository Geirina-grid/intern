/******************************************************************************
 * Copyright (c) 2015-2016, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenLib.cpp: a system library of token conversion function implementation. 
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
 * - The declaration is in TokenLib.hpp
 *
 * - You can unit test your token function in the main.cc
 *   To run your test, you can do 
 *
 *     g++ -I./ main.cpp TokenLib.cpp
 *     ./a.out
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: ZZ, MW
 ******************************************************************************/

#include <TokenLib.hpp>


 /**
  * this function concatenates all input tokens into one big token
  *
  */
extern "C" void gsql_concat(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char* const oToken, uint32_t& oTokenLen) {
      
  int k = 0;
  for (int i=0; i < iTokenNum; i++) {
    for (int j =0; j < iTokenLen[i]; j++) {
           oToken[k++] = iToken[i][j];
    }
  }
  oTokenLen = k;
}


/**
 * \brief  time convert utility
 */
class GtimeConv {
 public:

  /**
   * Only support 3 types
   * "%Y-%m-%d %H:%M:%S"
   * "%Y/%m/%d %H:%M:%S"
   * "%Y-%m-%dT%H:%M:%S.000z" // ignores text after .
   * e.g.
   * "2014-4-17 12:22:23"
   * "2014/4/17 12:22:23"
   *
   * @param s
   * @return
   */
  uint32_t seconds_from_epoch(const std::string& ymd_hms) {
    return seconds_from_epoch(ymd_hms.c_str(), ymd_hms.length());
  }

  uint32_t seconds_from_epoch(const char * ymd_hms, uint32_t length) {
    const int mon_days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273,
        304, 334, 365 };
    uint32_t tyears, tdays, leaps;
    char *timestamp_ptr_ = (char *) ymd_hms;
    char *timestamp_end_ptr_ = timestamp_ptr_ + length;
    struct tm t = { 0 };  // Initalize to all 0's
    t.tm_year = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_mon = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_mday = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_hour = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_min = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_sec = nextInt(timestamp_ptr_, timestamp_end_ptr_);

    tyears = t.tm_year - 1970;  // base time is 1970
    leaps = (tyears + 2) / 4;  // no of next two lines until year 2100.
    tdays = mon_days[t.tm_mon - 1];
    tdays += t.tm_mday - 1;  // days of month passed.
    tdays = tdays + (tyears * 365) + leaps;
    if (t.tm_mon <= 2 && ((tyears + 2)%4 == 0)) // leaf year: the 1st two months need -1 day.
      tdays--;
    return (tdays * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60)
        + t.tm_sec;
  }
 private:
  uint32_t nextInt(char *&timestamp_ptr, const char *timestamp_end_ptr) {
    uint32_t int_value = 0;
    while (*timestamp_ptr != ' ' && *timestamp_ptr != ':'
        && *timestamp_ptr != '-' && *timestamp_ptr != '/'
        && *timestamp_ptr != 'T' && *timestamp_ptr != '.'
        && *timestamp_ptr != '\n' && *timestamp_ptr != '\r'
        && timestamp_ptr < timestamp_end_ptr) {
      int_value = int_value * 10 + *timestamp_ptr - '0';
      timestamp_ptr++;
    }
    timestamp_ptr++;  // jump over separator
    return int_value;
  }
};

/* This funtion compares two strings, and returns true if they are equal */
static inline bool compareStrIgnoreCase (std::string s1, std::string s2) {

  int len = s1.length();
  if (len != s2.length()) {
    return false;
  }

  for (int i = 0; i < len; i++) {
    if (tolower(s1[i]) != tolower(s2[i])) {
      return false;
    }
  }

  return true;
}

/**
 * This function convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
 *
 * @param kdlsjfl
 * @return skdjfkl
 */

extern "C"  void gsql_reverse (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum, 
    char *const oToken, uint32_t& oTokenLen) {

  uint32_t j = 0;
  for (uint32_t i = iTokenLen[0]; i>0; i--) {
    oToken[j++] = iToken[0][i-1];     
  }
  oTokenLen = j;
}

extern "C" uint64_t gsql_ts_to_epoch_seconds(const char* const iToken[], uint32_t iTokenLen[], 
                                             uint32_t iTokenNum) {
  GtimeConv converter;
  return converter.seconds_from_epoch(iToken[0],iTokenLen[0]);
}

extern "C" void gsql_split_by_space (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
         char *const oToken, uint32_t& oTokenLen) {
  for(int i = 0; i < iTokenLen[0]; i++) {
    if(iToken[0][i] == ' '){
      oToken[i] = (char)30; 
    }else{
      oToken[i] = iToken[0][i]; 
    }
  }
  oToken[iTokenLen[0]] = (char)30;
  oTokenLen = iTokenLen[0]+1;
  
}

extern "C"  bool gsql_to_bool(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenLen[0] == 1 && (iToken[0][0] == 'T' || iToken[0][0] == 't')) {
    return true;
  }
 
  std::string trueString = "true";
  std::string token (iToken[0], iTokenLen[0]);
  return compareStrIgnoreCase(token, trueString);
}

extern "C" int64_t gsql_to_int(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum) {
  return (int64_t) atof(std::string(iToken[0], iTokenLen[0]).c_str());
}

extern "C" uint64_t gsql_to_uint(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum) {
  return (uint64_t) atof(std::string(iToken[0], iTokenLen[0]).c_str());
}

extern "C" uint64_t gsql_current_time_epoch(const char* const iToken[], uint32_t iTokenLen[],
                                            uint32_t iTokenNum) {
    return time(0);
}

/**
 * This function check whether the input string is "true" case insensitive;
 * return true if it is "true" or "t".
 */
extern "C"  bool gsql_is_true(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenLen[0] == 1 && (iToken[0][0] == 'T' || iToken[0][0] == 't')) {
    return true;
  }
 
  std::string trueString = "true";
  std::string token (iToken[0], iTokenLen[0]);
  return compareStrIgnoreCase(token, trueString);
}

/**
 * This function check whether the input string is "false" case insensitive;
 * return true if it is "false" or "f".
 */
extern "C"  bool gsql_is_false(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenLen[0] == 1 && (iToken[0][0] == 'F' || iToken[0][0] == 'f')) {
    return true;
  }
  std::string falseString = "false";
  std::string token (iToken[0], iTokenLen[0]);
  return compareStrIgnoreCase(token, falseString);
}

/**
 * This function checks whether the input string is empty 
 * after removing white space.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_not_empty_string($2);
 */
extern "C"  bool gsql_is_not_empty_string(const char* const iToken[], uint32_t iTokenLen[], 
                                          uint32_t iTokenNum) {
  if (iTokenLen[0] <= 0) {
    return false;
  }

  for (int i = 0; i < iTokenLen[0]; i++) {
    if (iToken[0][i] != ' ') {
      return true;
    }
  }
  //empty or all space, return false
  return false;
}

/**
 * This funtion compares two string, and returns true if they are equal.
 *   - if there isn't two string, return false.
 *   - if two string are not equal length, return false.
 *   - if any character is not equal, return false.
 */

extern "C" bool gsql_token_equal(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenNum != 2) {
    return false;
  }

  if (iTokenLen[0] != iTokenLen[1]) {
    return false;
  }

  for (int i =0; i < iTokenLen[0]; i++) {
    if (iToken[0][i] != iToken[1][i]) {
      return false;
    }
  }

  return true;
}

/**
 * This funtion compares two string in case insensitive, and returns true if they are equal.
 *   - if there isn't two string, return false.
 *   - if two string are not equal length, return false.
 *   - if any character is not equal, return false.
 *
 */

extern "C" bool gsql_token_ignore_case_equal(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenNum != 2) {
    return false;
  }

  if (iTokenLen[0] != iTokenLen[1]) {
    return false;
  }

  for (int i =0; i < iTokenLen[0]; i++) {
    if (tolower(iToken[0][i]) != tolower(iToken[1][i])) {
      return false;
    }
  }

  return true;
}

/**
 * Predicate testing helper function.
 *    E.g. WHERE $1 IS NUMERIC
 *    Will call this function.
 *
 * IsNumeric test whether a string is a numeric number, implemented as an FSM.
 *
 * Numeric number is defined as follows in regular expression:
 *      (+/-)?  [0-9]   [0-9]*    (.[0-9])?   [0-9]*  ((e/E) (+/-)?  [0-9]    [0-9]*)?
 *  ^     ^       ^       ^        ^   ^         ^       ^     ^       ^         ^         ^
 *  |     |       |       |        |   |         |       |     |       |         |         |
 * INIT  SIGN   START   MIDDLE    DOT  TAIL     TAIL    EXP  EXPSIGN EXPDGT  EXPDGTAIL    END
 *
 * Any space appear in between will not be taken as numeric number.
 * Space in front and at the end is OK.
 *
 * Author: QC
 * Created: July, 2015
 */

bool IsNumeric (std::string s) {
  enum State {INIT, SIGN, MIDDLE, DOT,
    TAIL, EXP, EXPSIGN, EXPDGT, EXPDGTAIL, END};
  size_t i = 0;
  State current = INIT;

  while (i < s.size()) {
    switch (current) {
      case INIT:
        if (s[i] == ' ') {
          // stays same
        } else if ( std::isdigit(s[i]) ) {
          current = MIDDLE;
        } else if (s[i] == '-' || s[i] == '+') {
          current = SIGN;
        } else if (s[i] == '.') {
          current = DOT;
        } else {
          return false;
        }
        break;
      case SIGN:
        if ( std::isdigit(s[i]) ) {
          current = MIDDLE;
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == '.') {
          current = DOT;
        } else {
          return false;
        }
        break;
      case MIDDLE:
        if ( std::isdigit(s[i]) ) {
          // stays same
        } else if (s[i] == '.') {
          current = DOT;
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == 'e' || s[i] == 'E') {
          current = EXP;
        } else {
          return false;
        }
        break;
      case DOT:
        if ( std::isdigit(s[i]) ) {
          current = TAIL;
        } else {
          return false;
        }
        break;
      case TAIL:
        if ( std::isdigit(s[i]) ) {
          // stays same
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == 'e' || s[i] == 'E') {
          current = EXP;
        } else {
          return false;
        }
        break;
      case EXP:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGT;
        } else if (s[i] == '-' || s[i] == '+') {
          current = EXPSIGN;
        } else {
          return false;
        }
        break;
      case EXPSIGN:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGT;
        } else {
          return false;
        }
        break;
      case EXPDGT:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGTAIL;
        } else if (s[i] == ' ') {
          current = END;
        } else {
          return false;
        }
        break;
      case EXPDGTAIL:
        if ( std::isdigit(s[i]) ) {
            ;// stays the same
        } else if (s[i] == ' ') {
          current = END;
        } else {
          return false;
        }
        break;
      case END:
        if (s[i] == ' ') {
          ;// stays the same
        } else {
          return false;
        }
        break;
      default:
        return false;
    }
    i ++;
  }

  if (current == TAIL
      || current == MIDDLE || current == END
      || current == EXPDGT || current == EXPDGTAIL) {
    return true;
  }
  return false;
}
