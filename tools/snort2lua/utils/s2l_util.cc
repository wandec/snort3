/*
** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
 * Copyright (C) 2002-2013 Sourcefire, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.  You may not use, modify or
 * distribute this program under any other version of the GNU General
 * Public License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
// s2l_util.cc author Josh Rosenbaum <jrosenba@cisco.com>

#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#include "utils/s2l_util.h"
#include "conversion_state.h"
#include "data/dt_data.h"

namespace util
{



std::vector<std::string> &split(const std::string &s, 
                                char delim, 
                                std::vector<std::string> &elems)
{
    std::istringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }

    return elems;
}

const ConvertMap* find_map(const std::vector<const ConvertMap*> map, std::string keyword)
{
    for (const ConvertMap *p : map)
        if (p->keyword.compare(0, p->keyword.size(), keyword) == 0)
            return p;

    return nullptr;
}

Table* find_table(std::vector<Table*> vec, std::string name)
{
    if(name.empty())
        return nullptr;

    for( auto *t : vec)
        if(!name.compare(t->get_name()))
            return t;

    return nullptr;
}



std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}



std::string &sanitize_lua_string(std::string &s)
{

    std::size_t found = s.find("]]");
    while (found != std::string::npos)
    {
        s.insert(found + 1, " ");
        found = s.find("]]");
    }

    found = s.find("  ");
    while (found != std::string::npos)
    {
        s.erase(found, 1);
        found = s.find("  ");

    }
    return s;
}


std::size_t get_substr_length(std::string str, std::size_t max_length)
{
    std::size_t str_len;

    if (str.size() < max_length)
        return str.size();

    str_len = str.rfind(" ", max_length);

    if (str_len == std::string::npos)
    {
        str_len = str.find(" ");

        if (str_len == std::string::npos)
            return str.size();
    }
    return str_len;
}

bool get_string(std::istringstream& stream,
                std::string& option,
                const std::string delimeters)
{
    if (delimeters.empty() || !stream.good())
    {
        option = std::string();
        return false;
    }
    else if (delimeters.size() == 1)
    {
        std::getline(stream, option, delimeters[0]);
        trim(option);
        return !option.empty();
    }
    else
    {
        std::streamoff pos = 0;
        option = std::string();

        // we don't want an empty string
        while (stream.good() && option.empty())
        {
            pos = stream.tellg();
            std::getline(stream, option, delimeters[0]);
        }

        // find the first non-delimeter charachter
        const std::size_t first_char = option.find_first_not_of(delimeters);

        // if there are no charachters between a delimeter, empty string. return false
        if (first_char == std::string::npos)
            return false;

        // find the first delimeter after the first non-delimeter
        std::size_t first_delim = option.find_first_of(delimeters, first_char);

        if (first_delim == std::string::npos)
            first_delim = option.size();    // set value to take proper substr
        else
            stream.seekg((std::streamoff)(pos) + (std::streamoff)(first_delim) + 1);


        option = option.substr(first_char, first_delim - first_char);
        trim(option);
        return true;
    }
}

std::string get_rule_option_args(std::istringstream& stream)
{
    std::string args = std::string();
    std::string tmp;

    do
    {
        std::getline(stream, tmp, ';');
        args += tmp + ";";

    } while (tmp.back() == '\\');

    // semicolon will be added when printing
    args.pop_back();
    trim(args);
    return args;
}

bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

bool case_compare(std::string arg1, std::string arg2)
{
    std::transform(arg1.begin(), arg1.end(), arg1.begin(), ::tolower);
    std::transform(arg2.begin(), arg2.end(), arg2.begin(), ::tolower);

    if (!arg1.compare(arg2))
        return true;
    return false;
}


} // namespace util
