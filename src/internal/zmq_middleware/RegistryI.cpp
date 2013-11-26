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

#include <scopes/internal/zmq_middleware/RegistryI.h>

#include <internal/zmq_middleware/capnproto/Registry.capnp.h>
#include <internal/zmq_middleware/capnproto/Scope.capnp.h>
#include <scopes/internal/zmq_middleware/ObjectAdapter.h>
#include <scopes/internal/zmq_middleware/VariantConverter.h>
#include <scopes/internal/zmq_middleware/ZmqScope.h>
#include <scopes/internal/RegistryObject.h>
#include <scopes/ScopeExceptions.h>

#include <cassert>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

/*

interface Scope;

dictionary<string, VariantMap> ScopeMap;

exception NotFoundException
{
    string scopeName;
};

interface Registry
{
    Scope* find(string name) throws NotFoundException;
    ScopeMap list();
};

*/

using namespace std::placeholders;

RegistryI::RegistryI(RegistryObject::SPtr const& ro) :
    ServantBase(ro, { { "find", bind(&RegistryI::find_, this, _1, _2, _3) },
                      { "list", bind(&RegistryI::list_, this, _1, _2, _3) } })

{
}

RegistryI::~RegistryI() noexcept
{
}

void RegistryI::find_(Current const&,
                      capnp::ObjectPointer::Reader& in_params,
                      capnproto::Response::Builder& r)
{
    auto req = in_params.getAs<capnproto::Registry::FindRequest>();
    string name = req.getName().cStr();
    auto delegate = dynamic_pointer_cast<RegistryObject>(del());
    try
    {
        auto meta = delegate->find(name);
        r.setStatus(capnproto::ResponseStatus::SUCCESS);
        auto find_response = r.initPayload().getAs<capnproto::Registry::FindResponse>().initResponse();
        auto dict = find_response.initReturnValue();
        to_value_dict(meta.serialize(), dict);
    }
    catch (NotFoundException const& e)
    {
        r.setStatus(capnproto::ResponseStatus::USER_EXCEPTION);
        auto find_response = r.initPayload().getAs<capnproto::Registry::FindResponse>().initResponse();
        find_response.initNotFoundException().setName(e.name().c_str());
    }
}

void RegistryI::list_(Current const&,
                      capnp::ObjectPointer::Reader&,
                      capnproto::Response::Builder& r)
{
    auto delegate = dynamic_pointer_cast<RegistryObject>(del());
    auto metadata_map = delegate->list();
    r.setStatus(capnproto::ResponseStatus::SUCCESS);
    auto rv = r.initPayload().getAs<capnproto::Registry::ListResponse>().initReturnValue(metadata_map.size());
    int i = 0;
    for (auto& pair : metadata_map)
    {
        auto dict = rv[i];
        to_value_dict(pair.second.serialize(), dict);
        ++i;
    }
}

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
