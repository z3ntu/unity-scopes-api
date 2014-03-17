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
 * along with this program.  If not, see <http://www.gnu.org/lzmqnses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/scopes/internal/zmq_middleware/RegistryI.h>

#include <unity/scopes/internal/RegistryConfig.h>
#include <unity/scopes/internal/RegistryException.h>
#include <unity/scopes/internal/RegistryObject.h>
#include <unity/scopes/internal/RuntimeImpl.h>
#include <unity/scopes/internal/ScopeMetadataImpl.h>
#include <unity/scopes/internal/ScopeImpl.h>
#include <unity/scopes/internal/UniqueID.h>
#include <unity/scopes/internal/zmq_middleware/ZmqRegistry.h>
#include <scopes/internal/zmq_middleware/capnproto/Message.capnp.h>
#include <unity/scopes/ScopeExceptions.h>
#include <unity/UnityExceptions.h>

#include <gtest/gtest.h>
#include <scope-api-testconfig.h>

#include <cassert>
#include <set>

using namespace std;
using namespace unity;
using namespace unity::scopes;
using namespace unity::scopes::internal;
using namespace unity::scopes::internal::zmq_middleware;

ScopeMetadata make_meta(const string& name, MWScopeProxy const& proxy, MiddlewareBase::SPtr const& mw)
{
    unique_ptr<ScopeMetadataImpl> mi(new ScopeMetadataImpl(mw.get()));
    mi->set_scope_id(name);
    mi->set_art("art " + name);
    mi->set_display_name("display name " + name);
    mi->set_description("description " + name);
    mi->set_author("author " + name);
    mi->set_search_hint("search hint " + name);
    mi->set_hot_key("hot key " + name);
    ScopeProxy p = ScopeImpl::create(proxy, mw->runtime(), name);
    mi->set_proxy(p);
    return ScopeMetadataImpl::create(move(mi));
}

TEST(RegistryI, get_metadata)
{
    RegistryObject::ScopeExecData dummy_exec_data;
    RuntimeImpl::UPtr runtime = RuntimeImpl::create(
        "TestRegistry", TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini");

    string identity = runtime->registry_identity();
    RegistryConfig c(identity, runtime->registry_configfile());
    string mw_kind = c.mw_kind();
    string mw_endpoint = c.endpoint();
    string mw_configfile = c.mw_configfile();

    MiddlewareBase::SPtr middleware = runtime->factory()->create(identity, mw_kind, mw_configfile);
    RegistryObject::SPtr ro(make_shared<RegistryObject>());
    auto registry = middleware->add_registry_object(identity, ro);
    auto p = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
    EXPECT_TRUE(ro->add_local_scope("scope1", move(make_meta("scope1", p, middleware)),
            dummy_exec_data));

    auto r = runtime->registry();
    auto scope = r->get_metadata("scope1");
    EXPECT_EQ("scope1", scope.scope_id());
}

TEST(RegistryI, list)
{
    RuntimeImpl::UPtr runtime = RuntimeImpl::create(
        "TestRegistry", TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini");

    string identity = runtime->registry_identity();
    RegistryConfig c(identity, runtime->registry_configfile());
    string mw_kind = c.mw_kind();
    string mw_endpoint = c.endpoint();
    string mw_configfile = c.mw_configfile();

    MiddlewareBase::SPtr middleware = runtime->factory()->create(identity, mw_kind, mw_configfile);
    RegistryObject::SPtr ro(make_shared<RegistryObject>());
    auto registry = middleware->add_registry_object(identity, ro);

    auto r = runtime->registry();
    auto scopes = r->list();
    EXPECT_TRUE(scopes.empty());

    RegistryObject::ScopeExecData dummy_exec_data;
    auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
    EXPECT_TRUE(ro->add_local_scope("scope1", move(make_meta("scope1", proxy, middleware)),
            dummy_exec_data));
    scopes = r->list();
    EXPECT_EQ(1u, scopes.size());
    EXPECT_NE(scopes.end(), scopes.find("scope1"));

    ro->remove_local_scope("scope1");
    scopes = r->list();
    EXPECT_EQ(0u, scopes.size());

    set<string> ids;
    for (int i = 0; i < 10; ++i)
    {
        string long_id = "0000000000000000000000000000000000000000000000" + to_string(i);
        EXPECT_TRUE(ro->add_local_scope(long_id, move(make_meta(long_id, proxy, middleware)),
                dummy_exec_data));
        ids.insert(long_id);
    }
    scopes = r->list();
    EXPECT_EQ(10u, scopes.size());
    for (auto& id : ids)
    {
        auto it = scopes.find(id);
        EXPECT_NE(scopes.end(), it);
        EXPECT_NE(ids.end(), ids.find(it->first));
    }
}

TEST(RegistryI, add_remove)
{
    RuntimeImpl::UPtr runtime = RuntimeImpl::create(
        "TestRegistry", TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini");

    string identity = runtime->registry_identity();
    RegistryConfig c(identity, runtime->registry_configfile());
    string mw_kind = c.mw_kind();
    string mw_endpoint = c.endpoint();
    string mw_configfile = c.mw_configfile();

    MiddlewareBase::SPtr middleware = runtime->factory()->create(identity, mw_kind, mw_configfile);
    RegistryObject::SPtr ro(make_shared<RegistryObject>());
    auto registry = middleware->add_registry_object(identity, ro);

    auto r = runtime->registry();
    auto scopes = r->list();
    EXPECT_TRUE(scopes.empty());

    RegistryObject::ScopeExecData dummy_exec_data;
    auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
    EXPECT_TRUE(ro->add_local_scope("scope1", move(make_meta("scope1", proxy, middleware)),
            dummy_exec_data));
    scopes = r->list();
    EXPECT_EQ(1u, scopes.size());
    EXPECT_NE(scopes.end(), scopes.find("scope1"));
    EXPECT_FALSE(ro->add_local_scope("scope1", move(make_meta("scope1", proxy, middleware)),
            dummy_exec_data));

    EXPECT_TRUE(ro->remove_local_scope("scope1"));
    scopes = r->list();
    EXPECT_EQ(0u, scopes.size());
    EXPECT_FALSE(ro->remove_local_scope("scope1"));

    set<string> ids;
    for (int i = 0; i < 10; ++i)
    {
        string long_id = "0000000000000000000000000000000000000000000000" + to_string(i);
        ro->add_local_scope(long_id, move(make_meta(long_id, proxy, middleware)),
                dummy_exec_data);
        ids.insert(long_id);
    }
    scopes = r->list();
    EXPECT_EQ(10u, scopes.size());
    for (auto& id : ids)
    {
        auto it = scopes.find(id);
        EXPECT_NE(scopes.end(), it);
        EXPECT_NE(ids.end(), ids.find(it->first));
    }
}

TEST(RegistryI, exceptions)
{
    RuntimeImpl::UPtr runtime = RuntimeImpl::create(
        "TestRegistry", TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini");

    string identity = runtime->registry_identity();
    RegistryConfig c(identity, runtime->registry_configfile());
    string mw_kind = c.mw_kind();
    string mw_endpoint = c.endpoint();
    string mw_configfile = c.mw_configfile();

    MiddlewareBase::SPtr middleware = runtime->factory()->create(identity, mw_kind, mw_configfile);
    RegistryObject::SPtr ro(make_shared<RegistryObject>());
    RegistryObject::ScopeExecData dummy_exec_data;
    auto registry = middleware->add_registry_object(identity, ro);
    auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
    ro->add_local_scope("scope1", move(make_meta("scope1", proxy, middleware)), dummy_exec_data);

    auto r = runtime->registry();

    try
    {
        r->get_metadata("fred");
        FAIL();
    }
    catch (NotFoundException const& e)
    {
        EXPECT_STREQ("unity::scopes::NotFoundException: Registry::get_metadata(): no such scope (name = fred)",
                     e.what());
    }

    try
    {
        r->get_metadata("");
        FAIL();
    }
    catch (MiddlewareException const& e)
    {
        EXPECT_STREQ("unity::scopes::MiddlewareException: unity::InvalidArgumentException: "
                     "RegistryObject::get_metadata(): Cannot search for scope with empty id",
                     e.what());
    }

    try
    {
        auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
        ro->add_local_scope("", move(make_meta("blah", proxy, middleware)), dummy_exec_data);
        FAIL();
    }
    catch (InvalidArgumentException const& e)
    {
        EXPECT_STREQ("unity::InvalidArgumentException: RegistryObject::add_local_scope(): Cannot add scope with empty id",
                     e.what());
    }

    try
    {
        ro->remove_local_scope("");
        FAIL();
    }
    catch (InvalidArgumentException const& e)
    {
        EXPECT_STREQ("unity::InvalidArgumentException: RegistryObject::remove_local_scope(): Cannot remove scope with empty id",
                     e.what());
    }
}

// RegistryObject that overrides the locate() method, so we can test it without having to run
// a full registry that spawns new processes.

class MockRegistryObject : public RegistryObject
{
public:
    MockRegistryObject()
    {
    }

    virtual ScopeProxy locate(string const& scope_id) override
    {
        if (scope_id == "no_such_scope")
        {
            throw NotFoundException("no can find", scope_id);
        }
        if (scope_id == "error_scope")
        {
            throw RegistryException("Couldn't start error_scope");
        }
        return get_metadata(scope_id).proxy();
    }
};

TEST(RegistryI, locate_mock)
{
    RuntimeImpl::UPtr runtime = RuntimeImpl::create(
        "TestRegistry", TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini");

    string identity = runtime->registry_identity();
    RegistryConfig c(identity, runtime->registry_configfile());
    string mw_kind = c.mw_kind();
    string mw_endpoint = c.endpoint();
    string mw_configfile = c.mw_configfile();
    RegistryObject::ScopeExecData dummy_exec_data;

    MiddlewareBase::SPtr middleware = runtime->factory()->create(identity, mw_kind, mw_configfile);
    MockRegistryObject::SPtr mro(make_shared<MockRegistryObject>());
    auto r = middleware->add_registry_object(identity, mro);
    auto r_proxy = dynamic_pointer_cast<ZmqRegistry>(r);
    auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
    mro->add_local_scope("scope1", move(make_meta("scope1", proxy, middleware)),
            dummy_exec_data);

    auto p = r_proxy->locate("scope1");
    EXPECT_EQ("scope1", p->identity());
    EXPECT_EQ("ipc:///tmp/scope1", p->endpoint());

    try
    {
        r_proxy->locate("no_such_scope");
        FAIL();
    }
    catch (NotFoundException const& e)
    {
        EXPECT_STREQ("unity::scopes::NotFoundException: Registry::locate(): no such scope (name = no_such_scope)",
                     e.what());
    }

    try
    {
        r_proxy->locate("error_scope");
        FAIL();
    }
    catch (RegistryException const& e)
    {
        EXPECT_STREQ("unity::scopes::RegistryException: Couldn't start error_scope", e.what());
    }

    try
    {
        auto proxy = middleware->create_scope_proxy("scope1", "ipc:///tmp/scope1");
        mro->add_local_scope("", move(make_meta("blah", proxy, middleware)),
                 dummy_exec_data);
        FAIL();
    }
    catch (InvalidArgumentException const& e)
    {
        EXPECT_STREQ("unity::InvalidArgumentException: RegistryObject::add_local_scope(): Cannot add scope with empty id",
                     e.what());
    }

    try
    {
        mro->remove_local_scope("");
        FAIL();
    }
    catch (InvalidArgumentException const& e)
    {
        EXPECT_STREQ("unity::InvalidArgumentException: RegistryObject::remove_local_scope(): Cannot remove scope with empty id",
                     e.what());
    }
}

int process_count()
{
    std::string cmd = "ps --ppid " + std::to_string(getpid()) + " | wc -l";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        return -1;
    }

    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);

    return stoi(result);
}

TEST(RegistryI, locate)
{
    // get number of processes belonging to this test instance
    int start_process_count = process_count();
    int current_process_count = 0;
    ASSERT_NE(-1, start_process_count);

    {
        // configure registry
        std::string rt_config = TEST_BUILD_ROOT "/gtest/scopes/internal/zmq_middleware/RegistryI/Runtime.ini";

        RuntimeImpl::UPtr rt = RuntimeImpl::create("TestRegistry", rt_config);
        string reg_id = rt->registry_identity();

        RegistryConfig c(reg_id, rt->registry_configfile());
        string mw_kind = c.mw_kind();
        string scoperunner_path = c.scoperunner_path();

        MiddlewareBase::SPtr mw = rt->factory()->find(reg_id, mw_kind);

        RegistryObject::SPtr reg(new RegistryObject);
        mw->add_registry_object(reg_id, reg);

        // configure scopes
        std::array<std::string, 6> scope_ids = {"scope-A", "scope-B", "scope-C", "scope-D", "scope-N", "scope-S"};
        std::map<std::string, ScopeProxy> proxies;

        for (auto& scope_id : scope_ids)
        {
            proxies[scope_id] = ScopeImpl::create(mw->create_scope_proxy(scope_id), mw->runtime(), scope_id);

            unique_ptr<ScopeMetadataImpl> mi(new ScopeMetadataImpl(mw.get()));
            mi->set_scope_id(scope_id);
            mi->set_display_name(scope_id);
            mi->set_description(scope_id);
            mi->set_author("Canonical Ltd.");
            mi->set_proxy(proxies[scope_id]);
            auto meta = ScopeMetadataImpl::create(move(mi));

            RegistryObject::ScopeExecData exec_data;
            exec_data.scope_id = scope_id;
            exec_data.scoperunner_path = scoperunner_path;
            exec_data.runtime_config = rt_config;
            exec_data.scope_config = TEST_BUILD_ROOT "/../demo/scopes/" + scope_id + "/" + scope_id + ".ini";

            reg->add_local_scope(scope_id, move(meta), exec_data);
        }

        // check that no scope processes are running
        for (auto const& scope_id : scope_ids)
        {
            EXPECT_FALSE(reg->is_scope_process_running(scope_id));
        }

        // check that no new processes have been started yet
        current_process_count = process_count();
        EXPECT_EQ(0, current_process_count - start_process_count);

        // locate scopes (start scope processes)
        for (auto const& scope_id : scope_ids)
        {
            EXPECT_EQ(proxies[scope_id], reg->locate(scope_id));
        }

        // check that all scopes processes are running
        for (auto const& scope_id : scope_ids)
        {
            EXPECT_TRUE(reg->is_scope_process_running(scope_id));
        }

        // check that 6 new processes were started
        current_process_count = process_count();
        EXPECT_EQ(6, current_process_count - start_process_count);

        // locate the second scope multiple times
        for (int i = 0; i < 1000; ++i)
        {
            EXPECT_EQ(proxies[scope_ids[1]], reg->locate(scope_ids[1]));
        }

        // check that no new processes were started
        current_process_count = process_count();
        EXPECT_EQ(6, current_process_count - start_process_count);

        // reg falls out of scope here and gets deleted (hense closing all scope processes)
    }

    // check that we are back to the original number of processes
    current_process_count = process_count();
    EXPECT_EQ(0, current_process_count - start_process_count);
}
