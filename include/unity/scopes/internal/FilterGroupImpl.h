/*
 * Copyright (C) 2016 Canonical Ltd
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

#pragma once

#include <string>
#include <map>
#include <unity/scopes/Variant.h>
#include <unity/scopes/FilterBase.h>

namespace unity
{

namespace scopes
{
class FilterState;

namespace internal
{

class FilterGroupImpl
{
public:
    FilterGroupImpl(std::string const& id, std::string const& label);
    std::string id() const;
    std::string label() const;
    static VariantArray serialize_filter_groups(Filters const& filters);
    static std::map<std::string, FilterGroup::SCPtr> deserialize_filter_groups(VariantArray const& var);

private:
    std::string id_;
    std::string label_;
};

} // namespace internal

} // namespace scopes

} // namespace unity
