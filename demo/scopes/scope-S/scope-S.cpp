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

#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/Category.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/Query.h>

#include <iostream>
#include <thread>

#define EXPORT __attribute__ ((visibility ("default")))

using namespace std;
using namespace unity::scopes;

// Example scope A: replies synchronously to a query. (Replies are returned before returning from the run() method.)

class MyQuery : public SearchQuery
{
public:
    MyQuery(Query const& query, CategoryRenderer const& renderer) :
        query_(query),
        renderer_(renderer)
    {
        cerr << "MyQuery/" << query.query_string() << " created" << endl;
    }

    ~MyQuery()
    {
        cerr << "MyQuery/" << query_.query_string() << " destroyed" << endl;
    }

    virtual void cancelled() override
    {
        cerr << "MyQuery/" << query_.query_string() << " cancelled" << endl;
    }

    virtual void run(SearchReplyProxy const& reply) override
    {
        cerr << "scope-slow: run called for \"" << query_.query_string() << "\"" << endl;
        this_thread::sleep_for(chrono::seconds(20));
        auto cat = reply->register_category("cat1", "Category 1", "", renderer_);
        CategorisedResult result(cat);
        result.set_title("scope-slow: result 1 for query \"" + query_.query_string() + "\"");
        reply->push(result);
        cout << "scope-slow: query \"" << query_.query_string() << "\" complete" << endl;
    }

private:
    Query query_;
    CategoryRenderer renderer_;
};

class MyScope : public ScopeBase
{
public:
    virtual int start(string const&, RegistryProxy const&) override
    {
        return VERSION;
    }

    virtual void stop() override {}

    virtual QueryBase::UPtr create_query(Query const& q, VariantMap const& hints) override
    {
        QueryBase::UPtr query(new MyQuery(q, renderer_));
        cout << "scope-slow: created query: \"" << q.query_string() << "\"" << endl;

        auto it = hints.find("cardinality");
        if (it != hints.end())
        {
            cerr << "result cardinality: " << it->second.get_int() << endl;
        }

        it = hints.find("locale");
        if (it != hints.end())
        {
            cerr << "locale: " << it->second.get_string() << endl;
        }

        return query;
    }

    virtual QueryBase::UPtr preview(Result const& result, VariantMap const&) override
    {
        cout << "scope-S: preview: \"" << result.uri() << "\"" << endl;
        return nullptr;
    }

private:
    CategoryRenderer renderer_;
};

extern "C"
{

    EXPORT
    unity::scopes::ScopeBase*
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_CREATE_FUNCTION()
    {
        return new MyScope;
    }

    EXPORT
    void
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base)
    {
        delete scope_base;
    }

}
