/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
 */

#include <unity/scopes/internal/ScopeConfig.h>
#include <unity/UnityExceptions.h>
#include <gtest/gtest.h>

using namespace unity::scopes;
using namespace unity::scopes::internal;

TEST(ScopeConfig, basic)
{
    {
        ScopeConfig cfg("configtest1.ini");

        EXPECT_EQ("Scope name", cfg.display_name());
        EXPECT_EQ("scope art", cfg.art());
        EXPECT_EQ("Canonical", cfg.author());
        EXPECT_EQ("an icon", cfg.icon());
        EXPECT_EQ("a search hint string", cfg.search_hint());
        EXPECT_EQ("a key", cfg.hot_key());

        auto attrs = cfg.display_attributes();
        EXPECT_EQ(2, attrs.size());
        EXPECT_EQ("foo", attrs["arbitrary_key"].get_string());
        EXPECT_EQ("bar", attrs["another_one"].get_string());
    }
    {
        ScopeConfig cfg("configtest2.ini");

        EXPECT_EQ("Scope name", cfg.display_name());
        EXPECT_EQ("scope art", cfg.art());
        EXPECT_EQ("Canonical", cfg.author());
        EXPECT_EQ("an icon", cfg.icon());
        EXPECT_EQ("a search hint string", cfg.search_hint());
        EXPECT_EQ("a key", cfg.hot_key());

        EXPECT_EQ(0, cfg.display_attributes().size());
    }
}
