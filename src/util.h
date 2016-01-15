#ifndef _UTIL_H_
#define _UTIL_H_

#include "json/json.h"

/** @brief Removes whitespace from the beginning and end of a string
 *  Whitespace is: SPACE ( ), TAB (\t), NEWLINE (\n), CARRIAGE RETURN (\r),
 *  FORM FEED (\f) and VERTICAL TAB (\v)
 *
 * @param input the string to remove whitespace from
 *
 * @return the trimmed string
 */
std::string trim(const std::string &input);


/** @brief Splits a string at a specified delimiter
 *
 * @param str the string to split
 * @param delimiter the character to split at
 * @param func if specified, store this function's result instead of the result tokens.
 *             it will be passed the result tokens one by one
 *
 * @return a vector of the result tokens between the delimiter
 */
std::vector<std::string> split(const std::string& str, char delimeter, std::string func (const std::string &)=NULL);


/** @brief returns the last element in a json array*/
Json::Value& last(Json::Value& elem);


/** @brief Finds the first item that satisfy a certain condition (predicate)
 * This is similar to the std::find_if function
 *
 * @param iterable the iterable that containes items
 * @param predicate a function pointer or object that accepts one parameter of type Contained
 *
 * @return a pointer to first item that satisfy the condition or nullptr
 */
template<typename Iterable, typename Predicate>
auto findFirstIf(Iterable& iterable, Predicate predicate) -> typename decltype(iterable.begin())::pointer {
    for (auto& contained : iterable) {
        if (predicate(contained)) {
            return &contained;
        }
    }
    return nullptr;
}


#endif
