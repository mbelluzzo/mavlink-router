/*
 * This file is part of the MAVLink Router project
 *
 * Copyright (C) 2016  Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "comm.h"
#include "endpoint.h"
#include "router.h"
#include "timeout.h"

struct endpoint_entry {
    struct endpoint_entry *next;
    Endpoint *endpoint;
    bool remove;
};

class Mainloop : public Router {
public:
    int open();
    int add_fd(int fd, void *data, int events);
    int mod_fd(int fd, void *data, int events);
    int remove_fd(int fd);
    void loop();
    void route_msg(struct buffer *buf, int target_sysid, int sender_sysid) override;
    void handle_read(Endpoint *e);
    void handle_canwrite(Endpoint *e);
    void handle_tcp_connection();
    int write_msg(Endpoint *e, const struct buffer *buf);
    void process_tcp_hangups();
    Timeout *add_timeout(uint32_t timeout_msec, bool (*cb)(void *data), const void *data);
    void del_timeout(Timeout *t);

    void free_endpoints(struct opt *opt);
    bool add_endpoints(Mainloop &mainloop, const char *uartstr, struct opt *opt);

    void print_statistics();

    int epollfd = -1;
    bool should_process_tcp_hangups = false;

private:
    endpoint_entry *g_tcp_endpoints;
    Endpoint **g_endpoints;
    int g_tcp_fd;

    Timeout *_timeouts = nullptr;

    int tcp_open(unsigned long tcp_port);
    void _del_timeouts();
};

struct endpoint_address {
    struct endpoint_address *next;
    const char *ip;
    unsigned long port;
};

struct opt {
    long unsigned baudrate;
    struct endpoint_address *ep_addrs;
    struct endpoint_address *master_addrs;
    unsigned long tcp_port;
    bool report_msg_statistics;
};
