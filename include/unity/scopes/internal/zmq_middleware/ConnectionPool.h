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
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#ifndef UNITY_SCOPES_INTERNAL_ZMQMIDDLEWARE_CONNECTIONPOOL_H
#define UNITY_SCOPES_INTERNAL_ZMQMIDDLEWARE_CONNECTIONPOOL_H

#include <unity/scopes/internal/zmq_middleware/RequestMode.h>
#include <unity/util/NonCopyable.h>

#include <zmqpp/socket.hpp>

#include <string>
#include <unordered_map>

// Simple connection pool for oneway invocations. Zmq sockets are not thread-safe, which means
// that a proxy cannot directly contain a socket because that would cause invocations on the same proxy by
// different threads to crash.
// So, we maintain a pool of invocation threads, with each thread keeping its own cache of sockets.
// Sockets are indexed by adapter name and created lazily.
//
// WARNING: No locking anywhere here. The pool is intended for us as a thread_local static member only.

namespace unity
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

class ConnectionPool final
{
public:
    NONCOPYABLE(ConnectionPool);
    ConnectionPool(zmqpp::context& context);
    ~ConnectionPool();
    zmqpp::socket& find(std::string const& endpoint);
    void remove(std::string const& endpoint);
    void register_socket(std::string const& endpoint, zmqpp::socket socket);

private:
    typedef std::unordered_map<std::string, zmqpp::socket> CPool;

    zmqpp::socket create_connection(std::string const& endpoint);

    zmqpp::context& context_;
    CPool pool_;
};

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace unity

#endif
