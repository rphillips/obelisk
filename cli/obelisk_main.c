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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "obelisk.h"
#include "obelisk_config.h"

obelisk_error_t* 
time_cb(json_t *params, json_t **result)
{
    time_t current_time = time(NULL);
    *result = json_integer(current_time);
    return OBELISK_SUCCESS;
}

obelisk_rpc_t rpc_callbacks[] = {
    {"time", time_cb}
};

void
usage();

int 
main(int argc, char **argv)
{
    int ch;
    obelisk_settings_t settings;
    obelisk_baton_t baton;

    obelisk_init(&settings);
    baton.settings = &settings;
    baton.rpc = &rpc_callbacks;
    baton.rpc_size = sizeof(rpc_callbacks);

    fprintf(stderr, "%s : JSON-RPC Server\n", PACKAGE_STRING);

    while (-1 != (ch = getopt(argc, argv,
                              "p:"
                              "l:"
                              "v"
                              "d"
                              "h"
                             ))) {
        switch (ch) {
            case 'p':
                settings.port = atoi(optarg);
                break;
            case 'l':
                settings.bindaddr = optarg;
                break;
            case 'v':
                settings.verbose++;
                break;
            case 'd':
                settings.daemonize = 1;
                break;
            case 'h':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    if (settings.verbose) {
        fprintf(stderr, "Listening on %s:%i\n", 
                settings.bindaddr ? settings.bindaddr : "0.0.0.0", 
                settings.port);
    }

    obelisk_run(&baton);

    return 0;
}

void
usage(const char *name)
{
    fprintf(stderr, "-p <num>      port (default:%i)\n", OBELISK_DEFAULT_PORT);
    fprintf(stderr, "-l <address>  bind address (default:all interfaces)\n");
    fprintf(stderr, "-d            run as a daemon (default: foreground)\n");
    fprintf(stderr, "-v            verbose\n");
    fprintf(stderr, "-vv           more verbosity\n");
    exit(0);
}

