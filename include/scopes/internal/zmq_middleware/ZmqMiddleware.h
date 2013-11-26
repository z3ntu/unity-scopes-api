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

#ifndef UNITY_API_SCOPES_INTERNAL_ZMQMIDDLEWARE_ZMQMIDDLEWARE_H
#define UNITY_API_SCOPES_INTERNAL_ZMQMIDDLEWARE_ZMQMIDDLEWARE_H

#include <scopes/internal/MiddlewareBase.h>
#include <scopes/internal/MWRegistryProxyFwd.h>
#include <scopes/internal/MWReplyProxyFwd.h>
#include <scopes/internal/ThreadPool.h>
#include <scopes/internal/UniqueID.h>
#include <scopes/internal/zmq_middleware/ZmqConfig.h>
#include <scopes/internal/zmq_middleware/ZmqObjectProxyFwd.h>

#include <zmqpp/context.hpp>

namespace unity
{

namespace api
{

namespace scopes
{

class ScopeBase;

namespace internal
{

class RuntimeImpl;

namespace zmq_middleware
{

class ObjectAdapter;
class ServantBase;

class ZmqMiddleware final : public MiddlewareBase
{
public:
    ZmqMiddleware(std::string const& server_name, std::string const& configfile, RuntimeImpl* runtime);
    virtual ~ZmqMiddleware() noexcept;

    virtual void start() override;
    virtual void stop() override;
    virtual void wait_for_shutdown() override;

    virtual MWProxy create_proxy(std::string const& identity, std::string const& endpoint) override;

    virtual MWRegistryProxy create_registry_proxy(std::string const& identity, std::string const& endpoint) override;
    virtual MWScopeProxy create_scope_proxy(std::string const& identity) override;
    virtual MWScopeProxy create_scope_proxy(std::string const& identity, std::string const& endpoint) override;

    virtual MWQueryCtrlProxy add_query_ctrl_object(QueryCtrlObject::SPtr const& ctrl) override;
    virtual MWQueryProxy add_query_object(QueryObject::SPtr const& query) override;
    virtual MWRegistryProxy add_registry_object(std::string const& identity, RegistryObject::SPtr const& registry) override;
    virtual MWReplyProxy add_reply_object(ReplyObject::SPtr const& reply) override;
    virtual MWScopeProxy add_scope_object(std::string const& identity, ScopeObject::SPtr const& scope) override;

    zmqpp::context* context() const noexcept;
    ThreadPool* invoke_pool() const noexcept;

private:
    std::shared_ptr<ObjectAdapter> find_adapter(std::string const& name, std::string const& endpoint_dir);

    ZmqProxy safe_add(std::function<void()>& disconnect_func,
                      std::shared_ptr<ObjectAdapter> const& adapter,
                      std::string const& identity,
                      std::shared_ptr<ServantBase> const& servant);

    std::string server_name_;

    typedef std::map<std::string, std::shared_ptr<ObjectAdapter>> AdapterMap;
    AdapterMap am_;

    std::unique_ptr<ThreadPool> invokers_;
    mutable std::mutex mutex_;
    zmqpp::context context_;
    UniqueID unique_id_;

    enum State { Stopped, Starting, Started };
    State state_;
    std::condition_variable state_changed_;

    ZmqConfig config_;
};

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity

#endif
