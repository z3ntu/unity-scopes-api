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

#include <unity/scopes/internal/QueryMetadataImpl.h>

namespace unity
{

namespace scopes
{

namespace internal
{

QueryMetadataImpl::QueryMetadataImpl(std::string const& locale, std::string const& form_factor, GeoCoordinate const& location)
    : locale_(locale),
      form_factor_(form_factor),
      location_(location)
{
}

std::string QueryMetadataImpl::locale() const
{
    return locale_;
}

std::string QueryMetadataImpl::form_factor() const
{
    return form_factor_;
}

GeoCoordinate QueryMetadataImpl::location() const
{
    return location_;
}

} // namespace internal

} // namespace scopes

} // namespace unity
