/*
   Copyright 2010 Ryan Phillips

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "obelisk.h"

json_t*
obelisk_json_response(json_t *result, json_t *id)
{
    json_t *response;
    json_t *rpc_version = json_string("2.0");

    response = json_object();
    json_object_set(response, "jsonrpc", rpc_version);
    json_object_set(response, "result", result);
    json_object_set(response, "id", id);

    json_decref(rpc_version);
    /* id does not need to be decref'ed since it is a shared reference */

    return response;
}

