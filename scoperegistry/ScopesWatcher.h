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

#include <DirWatcher.h>

#include <unity/scopes/internal/RegistryObject.h>

namespace scoperegistry
{

// ScopesWatcher watches the scope install directories specified by calls to add_install_dir() for
// the installation / uninstallation of scopes. If a scope is removed, the registry is informed
// accordingly. If a scope is added, a user callback (provided on construction) is executed.

class ScopesWatcher : public DirWatcher
{
public:
    ScopesWatcher(unity::scopes::internal::RegistryObject::SPtr registry,
                  std::function<void(std::pair<std::string, std::string> const&)> ini_added_callback,
                  unity::scopes::internal::Logger& logger);

    ~ScopesWatcher();

    void add_install_dir(std::string const& dir);

private:
    unity::scopes::internal::RegistryObject::SPtr const registry_;
    std::function<void(std::pair<std::string, std::string> const&)> const ini_added_callback_;
    unity::scopes::internal::Logger& logger_;
    std::map<std::string, std::string> sdir_to_ini_map_;
    std::map<std::string, std::set<std::string>> idir_to_sdirs_map_;
    std::mutex mutex_;

    static std::string parent_dir(std::string const& child_dir);

    void remove_install_dir(std::string const& dir);

    void add_scope_dir(std::string const& dir);
    void remove_scope_dir(std::string const& dir);

    void watch_event(DirWatcher::EventType event_type,
                     DirWatcher::FileType file_type,
                     std::string const& path) override;
};

} // namespace scoperegistry
