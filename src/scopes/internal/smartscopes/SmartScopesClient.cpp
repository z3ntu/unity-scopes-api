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
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
 */

#include <unity/scopes/internal/smartscopes/SmartScopesClient.h>
#include <unity/UnityExceptions.h>

#include <algorithm>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <cstring>

static const std::string c_base_url = "https://productsearch.ubuntu.com/smartscopes/v2";
static const std::string c_remote_scopes_resource = "/remote-scopes";
static const std::string c_search_resource = "/search";

using namespace unity::scopes;
using namespace unity::scopes::internal::smartscopes;

//-- SearchHandle

SearchHandle::SearchHandle(std::string const& session_id, SmartScopesClient::SPtr ssc)
    : session_id_(session_id)
    , ssc_(ssc)
{
}

SearchHandle::~SearchHandle()
{
    cancel_search();
}

std::vector<SearchResult> SearchHandle::get_search_results()
{
    return ssc_->get_search_results(session_id_);
}

void SearchHandle::cancel_search()
{
    ssc_->cancel_search(session_id_);
}

//-- SmartScopesClient

SmartScopesClient::SmartScopesClient(HttpClientInterface::SPtr http_client,
                                     JsonNodeInterface::SPtr json_node,
                                     std::string const& url,
                                     uint port)
    : http_client_(http_client)
    , json_node_(json_node)
    , url_(url)
    , port_(port)
{
    if (url_.empty())
    {
        char* base_url_env = ::getenv("SMART_SCOPES_SERVER");
        std::string base_url = base_url_env ? base_url_env : "";
        if (!base_url.empty())
        {
            // find the last occurrence of ':' in the url in order to extract the port number
            // * ignore the colon after "http"/"https"

            const size_t hier_pos = strlen("https");

            uint64_t found = base_url.find_last_of(':');
            if (found != std::string::npos && found > hier_pos)
            {
                url_ = base_url.substr(0, found);
                port_ = std::stoi(base_url.substr(found + 1));
            }
            else
            {
                url_ = base_url;
            }
        }
        else
        {
            url_ = c_base_url;
        }
    }
}

SmartScopesClient::~SmartScopesClient()
{
}

std::vector<RemoteScope> SmartScopesClient::get_remote_scopes(std::string const& locale)
{
    try
    {
        std::ostringstream remote_scopes_uri;
        remote_scopes_uri << url_ << c_remote_scopes_resource << "?";

        // optional parameters

        if (!locale.empty())
        {
            remote_scopes_uri << "&locale=\"" << locale << "\"";
        }

        std::string response_str;
        std::cout << "SmartScopesClient: GET " << remote_scopes_uri.str() << std::endl;
        HttpResponseHandle::SPtr response = http_client_->get(remote_scopes_uri.str(), port_);
        response->wait();

        response_str = response->get();
        std::cout << "SmartScopesClient: Remote scopes:" << std::endl << response_str << std::endl;

        std::vector<RemoteScope> remote_scopes;
        JsonNodeInterface::SPtr root_node;
        JsonNodeInterface::SPtr child_node;
        RemoteScope scope;

        {
            std::lock_guard<std::mutex> lock(json_node_mutex_);
            json_node_->read_json(response_str);
            root_node = json_node_->get_node();
        }

        for (int i = 0; i < root_node->size(); ++i)
        {
            child_node = root_node->get_node(i);

            if (!child_node->has_node("name") || !child_node->has_node("base_url") ||
                !child_node->has_node("description"))
            {
                break;
            }

            scope.name = child_node->get_node("name")->as_string();
            scope.description = child_node->get_node("description")->as_string();
            scope.base_url = child_node->get_node("base_url")->as_string();

            scope.invisible = child_node->has_node("invisible") ? child_node->get_node("invisible")->as_bool() : false;

            remote_scopes.push_back(scope);
        }

        std::cout << "SmartScopesClient: Retrieved remote scopes from uri: " << url_ << c_remote_scopes_resource << std::endl;
        return remote_scopes;
    }
    catch (unity::Exception const& e)
    {
        std::cout << "SmartScopesClient: Failed to retrieve remote scopes from uri: " << url_ << c_remote_scopes_resource << std::endl;
        throw;
    }
}

SearchHandle::UPtr SmartScopesClient::search(std::string const& base_url,
                                             std::string const& query,
                                             std::string const& session_id,
                                             uint query_id,
                                             std::string const& platform,
                                             std::string const& locale,
                                             std::string const& country,
                                             uint limit)
{
    std::ostringstream search_uri;
    search_uri << base_url << c_search_resource << "?";

    // mandatory parameters

    search_uri << "q=\"" << http_client_->to_percent_encoding(query) << "\"";
    search_uri << "&session_id=\"" << session_id << "\"";
    search_uri << "&query_id=" << std::to_string(query_id);
    search_uri << "&platform=\"" << platform << "\"";

    // optional parameters

    if (!locale.empty())
    {
        search_uri << "&locale=" << locale;
    }
    if (!country.empty())
    {
        search_uri << "&country=" << country;
    }
    if (limit != 0)
    {
        search_uri << "&limit=" << std::to_string(limit);
    }

    cancel_search(session_id);

    std::lock_guard<std::mutex> lock(search_results_mutex_);
    std::cout << "SmartScopesClient: GET " << search_uri.str() << std::endl;
    search_results_[session_id] = http_client_->get(search_uri.str(), port_);

    return SearchHandle::UPtr(new SearchHandle(session_id, shared_from_this()));
}

std::vector<SearchResult> SmartScopesClient::get_search_results(std::string const& session_id)
{
    try
    {
        std::string response_str;

        {
            std::lock_guard<std::mutex> lock(search_results_mutex_);

            auto it = search_results_.find(session_id);
            if (it == search_results_.end())
            {
                throw unity::LogicException("No search for session " + session_id + " is active");
            }

            search_results_[session_id]->wait();

            response_str = search_results_[session_id]->get();
            std::cout << "SmartScopesClient: Search:" << std::endl << response_str << std::endl;
            search_results_.erase(it);
        }

        std::vector<SearchResult> results;
        std::map<std::string, std::shared_ptr<SearchCategory>> categories;

        std::vector<std::string> jsons = extract_json_stream(response_str);

        for (std::string& json : jsons)
        {
            JsonNodeInterface::SPtr root_node;
            JsonNodeInterface::SPtr child_node;

            {
                std::lock_guard<std::mutex> lock(json_node_mutex_);
                json_node_->read_json(json);
                root_node = json_node_->get_node();
            }

            if (root_node->has_node("category"))
            {
                child_node = root_node->get_node("category");
                auto category = std::make_shared<SearchCategory>();

                std::vector<std::string> members = child_node->member_names();
                for (auto& member : members)
                {
                    if (member == "icon")
                    {
                        category->icon = child_node->get_node(member)->as_string();
                    }
                    else if (member == "id")
                    {
                        category->id = child_node->get_node(member)->as_string();
                    }
                    else if (member == "render_template")
                    {
                        category->renderer_template = child_node->get_node(member)->as_string();
                    }
                    else if (member == "title")
                    {
                        category->title = child_node->get_node(member)->as_string();
                    }
                }

                categories[category->id] = category;
            }
            else if (root_node->has_node("result"))
            {
                child_node = root_node->get_node("result");
                SearchResult result;

                std::vector<std::string> members = child_node->member_names();
                for (auto& member : members)
                {
                    if (member == "art")
                    {
                        result.art = child_node->get_node(member)->as_string();
                    }
                    else if (member == "dnd_uri")
                    {
                        result.dnd_uri = child_node->get_node(member)->as_string();
                    }
                    else if (member == "title")
                    {
                        result.title = child_node->get_node(member)->as_string();
                    }
                    else if (member == "uri")
                    {
                        result.uri = child_node->get_node(member)->as_string();
                    }
                    else if (member == "cat_id")
                    {
                        std::string category = child_node->get_node(member)->as_string();
                        result.category = categories.find(category) != categories.end() ? categories[category] : nullptr;
                    }
                    else
                    {
                        result.other_params[member] = child_node;
                    }
                }

                results.push_back(result);
            }
        }

        std::cout << "SmartScopesClient: Retrieved search results for session: " << session_id << std::endl;
        return results;
    }
    catch (unity::Exception const& e)
    {
        std::cout << "SmartScopesClient: Failed to retrieve search results for session: " << session_id << std::endl;
        throw;
    }
}

std::vector<std::string> SmartScopesClient::extract_json_stream(std::string const& json_stream)
{
    std::vector<std::string> jsons;

    uint start_pos = 0;

    while (start_pos < json_stream.size())
    {
        int end_pos = json_stream.find("\r\n", start_pos);
        if (end_pos == -1)
        {
            end_pos = json_stream.size();
        }

        std::string sub_json_str = json_stream.substr(start_pos, end_pos - start_pos);
        jsons.push_back(sub_json_str);
        start_pos = end_pos + 2;
    }

    return jsons;
}

void SmartScopesClient::cancel_search(std::string const& session_id)
{
    std::lock_guard<std::mutex> lock(search_results_mutex_);

    auto it = search_results_.find(session_id);
    if (it != search_results_.end())
    {
        http_client_->cancel_get(search_results_[session_id]);
        search_results_.erase(it);
    }
}
