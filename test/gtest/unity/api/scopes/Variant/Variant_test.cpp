/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/api/scopes/Variant.h>
#include <unity/UnityExceptions.h>

#include <gtest/gtest.h>
#include <boost/variant.hpp>

using namespace std;
using namespace unity;
using namespace unity::api::scopes;

TEST(Variant, basic)
{
    {
        Variant v;
        EXPECT_EQ(Variant::Type::Int, v.which());
        v.get_int();
        // No exception here means that v was initialized to an int with unknown value.
    }

    {
        Variant v(42);
        EXPECT_EQ(Variant::Type::Int, v.which());
        EXPECT_EQ(42, v.get_int());
    }

    {
        Variant v(true);
        EXPECT_EQ(Variant::Type::Bool, v.which());
        EXPECT_EQ(true, v.get_bool());
    }

    {
        Variant v(string("hello"));
        EXPECT_EQ(Variant::Type::String, v.which());
        EXPECT_EQ("hello", v.get_string());
    }

    {
        Variant v("hello");
        EXPECT_EQ(Variant::Type::String, v.which());
        EXPECT_EQ("hello", v.get_string());
    }

    {
        // Copy constructor
        Variant v(42);
        Variant v2(v);
        EXPECT_EQ(42, v2.get_int());
    }

    {
        // Assignment operators

        Variant v("hello");
        Variant v2(42);

        v = v2;
        EXPECT_EQ(42, v.get_int());

        v = 99;
        EXPECT_EQ(99, v.get_int());

        v = false;
        EXPECT_FALSE(v.get_bool());

        v = "hello";
        EXPECT_EQ("hello", v.get_string());

        v = string("world");
        EXPECT_EQ("world", v.get_string());
    }

    {
        // Comparison operators

        Variant v1(1);
        Variant v2(2);
        EXPECT_TRUE(v1 < v2);
        EXPECT_FALSE(v1 == v2);

        v2 = "hello";
        EXPECT_TRUE(v1 < v2);   // Any int is less than any string
        EXPECT_FALSE(v2 < v1);
        EXPECT_TRUE(v1 < v2);
        EXPECT_FALSE(v1 == v2);

        v1 = v2;
        EXPECT_FALSE(v1 < v2);
        EXPECT_FALSE(v2 < v1);
        EXPECT_TRUE(v1 == v2);

        v1 = 5;
        v2 = false;
        EXPECT_TRUE(v1 < v2);   // Any int is less than any bool

        v1 = true;
        v2 = "";
        EXPECT_TRUE(v1 < v2);   // Any bool is less than any string
    }

    {
        // swap

        Variant v1(1);
        Variant v2(2);
        swap(v1, v2);
        EXPECT_EQ(2, v1.get_int());
        EXPECT_EQ(1, v2.get_int());

        v2 = "hello";
        swap(v1, v2);
        EXPECT_EQ("hello", v1.get_string());
        EXPECT_EQ(2, v2.get_int());
    }
}

TEST(Variant, exceptions)
{
    try
    {
        Variant v;
        v.get_bool();
        FAIL();
    }
    catch (LogicException const& e)
    {
        EXPECT_EQ("unity::LogicException: Variant does not contain a bool value:\n"
                  "    boost::bad_get: failed value get using boost::get",
                  e.to_string());
    }

    try
    {
        Variant v;
        v.get_string();
        FAIL();
    }
    catch (LogicException const& e)
    {
        EXPECT_EQ("unity::LogicException: Variant does not contain a string value:\n"
                  "    boost::bad_get: failed value get using boost::get",
                  e.to_string());
    }

    try
    {
        Variant v("hello");
        v.get_int();
        FAIL();
    }
    catch (LogicException const& e)
    {
        EXPECT_EQ("unity::LogicException: Variant does not contain an int value:\n"
                  "    boost::bad_get: failed value get using boost::get",
                  e.to_string());
    }

}