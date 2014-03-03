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

#ifndef UNITY_SCOPES_OBJECTPROXYFWD_H
#define UNITY_SCOPES_OBJECTPROXYFWD_H

#include <memory>

namespace unity
{

namespace scopes
{

class ObjectProxy;
/** \typedef Proxy
\brief Convenience type definition for the proxy inheritance root.
*/
typedef std::shared_ptr<ObjectProxy> Proxy;

} // namespace scopes

} // namespace unity

#endif
