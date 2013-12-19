#!/usr/bin/env python

#
# Copyright (C) 2013 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
#

from wsgiref.simple_server import make_server
import sys

def response(environ, start_response):
    status = '200 OK'
    response_headers = [('Content-Type', 'application/json')]
    start_response(status, response_headers)

    if environ['PATH_INFO'] == '/smartscopes/v2/remote-scopes':
        return [remote_scopes_response]

    if environ['PATH_INFO'] == '/smartscopes/v2/search/demo' and environ['QUERY_STRING'] != '':
        return [search_response]

    return ''

remote_scopes_response = '\
[{"search_url": "https://productsearch.ubuntu.com/smartscopes/v2/search/demo", "name": "Dummy Demo Scope" },\
{"search_url": "https://productsearch.ubuntu.com/smartscopes/v2/search/demo2", "name": "Dummy Demo Scope 2", "invisible": true }]'

search_response = '\
{"category": {"renderer_template": "", "id": "cat1", "title": "Category 1"}}\r\n\
{"result": {"cat_id": "cat1", "art": "https://productsearch.ubuntu.com/imgs/amazon.png", "uri": "URI", "title": "Stuff"}}\r\n\
{"result": {"cat_id": "cat1", "art": "https://productsearch.ubuntu.com/imgs/google.png", "uri": "URI2", "title": "Things"}}'

serving = False
port = 1024
while serving == False:
    try:
        httpd = make_server('127.0.0.1', port, response)
        serving = True
    except:
        port += 1

print(str(port))
sys.stdout.close()

httpd.serve_forever()
