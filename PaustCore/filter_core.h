#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <json/json.h>

#include "javascript_engine.h"

#include "ffxiv_data.h"

class filter_core
{
private:
    void pipe_thread();

    bool config_apply         (const Json::Value& jo, std::string* error);
    bool config_test_javascriptr(const std::string& script, std::string* error);

    HANDLE           ipc_pipe_hwnd = { 0, };
    std::atomic_bool ipc_thread_working = false;
    std::thread      ipc_thread;

    std::shared_mutex config_lock;
    std::atomic_bool  config_shortname = false;
    std::atomic_bool  config_filter = false;
    std::string       config_filter_javascript;

    std::unique_ptr<javascript_engine> javascript;

public:
    filter_core();
    ~filter_core();

    void modify_packet(party_finder_packet* packet);
};

