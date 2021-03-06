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
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
*/

#pragma once

#include <unity/scopes/CompletionDetails.h>

namespace unity
{

namespace scopes
{

namespace internal
{

class CompletionDetailsImpl final
{
public:
    CompletionDetailsImpl(CompletionDetails::CompletionStatus status);
    CompletionDetailsImpl(CompletionDetails::CompletionStatus status, std::string const& message);

    CompletionDetailsImpl(CompletionDetailsImpl const&) = default;
    CompletionDetailsImpl(CompletionDetailsImpl&&) = default;

    CompletionDetailsImpl& operator=(CompletionDetailsImpl const&) = default;
    CompletionDetailsImpl& operator=(CompletionDetailsImpl&&) = default;

    CompletionDetails::CompletionStatus status() const noexcept;
    std::string message() const;
    void add_info(OperationInfo const& info);
    std::vector<OperationInfo> info_list() const;

private:
    CompletionDetails::CompletionStatus status_;
    std::string message_;
    std::vector<OperationInfo> info_list_;
};

} // namespace internal

} // namespace scopes

} // namespace unity
