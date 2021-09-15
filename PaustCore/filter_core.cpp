#include "filter_core.h"

#include <random>
#include <vector>
#include <memory>

#include "debug_log.h"
#include "utils.h"
#include "defer.h"
#include "ffxiv_data.h"

bool filter_core::config_apply(const Json::Value& jo, std::string* error)
{
    std::unique_lock<std::shared_mutex> _(this->config_lock);

    try
    {
        const auto enabledShortName = jo["shortname_enabled"].asBool();

        if (jo["filter_enabled"].asBool())
        {
            const auto script = jo["filter_script"].asString();

            const auto ret = this->config_test_javascriptr(script, error);
            if (ret)
            {
                this->config_filter = true;
                this->config_filter_javascript = script;
            }
            else
            {
                return false;
            }
        }
        else
        {
            this->config_filter = false;
        }

        this->config_shortname = enabledShortName;

        return true;
    }
    catch (const Json::Exception& ex)
    {
        *error = ex.what();
    }

    return false;
}

bool filter_core::config_test_javascriptr(const std::string& script, std::string* error)
{
    /*
    debug_log(L"config_test_javascriptr script - raw : %ws", to_hex_wstring((const uint8_t*)script.data(), script.size()).c_str());
    debug_log(L"config_test_javascriptr script       : %ws", utf8_to_wstring(script).c_str());
    */
    debug_log(L"config_test_javascriptr script : %ws", utf8_to_wstring(script).c_str());

    error->clear();

    auto js = std::make_unique<javascript_engine>();
    
    bool succeed;
    js->eval(party_finder_packet_item_sample, script, &succeed, error);

    return succeed;
}

filter_core::filter_core()
{
    this->javascript = std::make_unique<javascript_engine>();
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    std::wstring pipeName = L"\\\\.\\pipe\\PaustCore\\" + std::to_wstring(GetCurrentProcessId());
    debug_log(L"Pipe Name : %ws", pipeName.c_str());

    this->ipc_pipe_hwnd = CreateNamedPipeW(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        BUFSIZ,
        BUFSIZ,
        5000,
        NULL
    );
    debug_log(L"CreateNamedPipeW : %x (%d)", this->ipc_pipe_hwnd, GetLastError());

    this->ipc_thread_working = true;
    this->ipc_thread = std::thread(&filter_core::pipe_thread, this);
}

void filter_core::modify_packet(party_finder_packet* packet)
{
    std::shared_lock<std::shared_mutex> _(this->config_lock);

    for (int i = 0; i < packet->items.size(); i++)
    {
        auto item = &packet->items[i];
        if (item->party_id == 0)
        {
            continue;
        }

        /*
        {
            // DUMP PAKCET ITEM
            debug_log(L"----------------------------------------------------------------------");

            const auto hex = to_hex_wstring((const uint8_t*)item, sizeof(*item));
            for (size_t k = 0; k < hex.size(); k += 60)
            {
                debug_log(hex.substr(k, 60));
            }
        }
        */

        if (this->config_filter)
        {
            if (this->javascript->eval(item, this->config_filter_javascript, NULL, NULL) == false)
            {
                memset(item, 0, sizeof(party_finder_packet_item));
                continue;
            }
        }

        if (this->config_shortname)
        {
            std::wstring name_w = utf8_to_wstring((const char*)item->owner_name.data());
            if (name_w.size() >= 5)
            {
                size_t len = 0;
                int char_width = 0;
                while (len < name_w.size())
                {
                    const auto c = name_w.at(len);

                    char_width += (0 <= c && c <= 128) ? 1 : 2;
                    if (char_width >= 7)
                    {
                        break;
                    }

                    len++;
                }
                
                if (len < name_w.size())
                {
#ifdef _DEBUG
                    const auto name_w_new = name_w.substr(0, len) + L"..";
                    debug_log(L"%ws -> %ws", name_w.c_str(), name_w_new.c_str());
                    name_w = name_w_new;
#else
                    name_w = name_w.substr(0, len) + L"..";
#endif
                    auto neme_new_utf8 = wstring_to_utf8(name_w);

                    item->owner_name.fill(0);
                    neme_new_utf8.copy((char*)item->owner_name.data(), neme_new_utf8.size(), 0);
                }
            }
        }
    }
}

filter_core::~filter_core()
{
    debug_log(L"DisconnectNamedPipe ipc_hwnd");
    DisconnectNamedPipe(this->ipc_pipe_hwnd);

    debug_log(L"CloseHandle ipc_hwnd");
    CloseHandle(this->ipc_pipe_hwnd);

    debug_log(L"ipc_thread_working");
    this->ipc_thread_working = false;

    try
    {
        debug_log(L"detach");
        this->ipc_thread.detach();
    }
    catch (...)
    {
    }

    debug_log(L"destroy Javascript-Engine");
    
    this->javascript.reset();
}

void filter_core::pipe_thread()
{
#pragma warning(disable : 6387)
    constexpr size_t BUFFER_SIZE = 4096;

    auto recv_buf = std::make_unique<char[]>(BUFFER_SIZE);
    std::string recv_buf_full;
    recv_buf_full.resize(BUFFER_SIZE);

    Json::CharReaderBuilder                 jo_input_reader_builder;
    const std::unique_ptr<Json::CharReader> jo_input_reader(jo_input_reader_builder.newCharReader());
    Json::Value                             jo_input;

    Json::StreamWriterBuilder jo_out_writer_builder;
    jo_out_writer_builder["commentStyle"] = "None";
    jo_out_writer_builder["indentation"] = "";

    const std::unique_ptr<Json::StreamWriter> jo_out_writer(jo_out_writer_builder.newStreamWriter());

    OVERLAPPED ol = { 0, };
    DWORD read = 0;
    bool ret = false;

#define ASYNC(FUNC)                                                                             \
    {                                                                                           \
        ol.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);                                      \
        ret = FUNC == TRUE;                                                                     \
        if (!ret)                                                                               \
        {                                                                                       \
            switch (GetLastError())                                                             \
            {                                                                                   \
            case ERROR_PIPE_CONNECTED:                                                          \
                ret = true;                                                                     \
                break;                                                                          \
                                                                                                \
            case ERROR_IO_PENDING:                                                              \
                if (WaitForSingleObject(ol.hEvent, 200) == WAIT_OBJECT_0)                       \
                {                                                                               \
                    ret = GetOverlappedResult(this->ipc_pipe_hwnd, &ol, &read, FALSE) == TRUE;  \
                }                                                                               \
                else                                                                            \
                {                                                                               \
                    CancelIo(this->ipc_pipe_hwnd);                                              \
                }                                                                               \
                break;                                                                          \
            default:                                                                            \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
        CloseHandle(ol.hEvent);                                                                 \
    }

    while (this->ipc_thread_working)
    {
        recv_buf_full.clear();

        ASYNC(ConnectNamedPipe(this->ipc_pipe_hwnd, &ol));
        if (!ret) continue;
        debug_log(L"pipe_thread ConnectNamedPipe: ret: %d", ret ? 1 : 0);

        do
        {
            ASYNC(ReadFile(this->ipc_pipe_hwnd, recv_buf.get(), BUFFER_SIZE, &read, &ol));
            debug_log(L"pipe_thread ReadFile / ret: %d / read: %d / body: %d", ret ? 1 : 0, read, recv_buf_full.size());

            if (ret && read > 0)
            {
                recv_buf_full.append(recv_buf.get(), static_cast<size_t>(read));

                if (recv_buf[static_cast<size_t>(read) - 1] == 0) break;
            }
        } while (ret);

        debug_log(L"pipe_thread body_buf     : %ws", utf8_to_wstring(recv_buf_full).c_str());
        debug_log(L"pipe_thread body_buf raw : %ws", to_hex_wstring((const uint8_t*)recv_buf_full.data(), recv_buf_full.size()).c_str());

        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ret = false;

        std::string jo_out_error;
        if (!jo_input_reader->parse(recv_buf_full.c_str(), recv_buf_full.c_str() + recv_buf_full.size(), &jo_input, &jo_out_error))
        {
            debug_log(L"pipe_thread json error : %ws", to_wstring(jo_out_error).c_str());
        }
        else
        {
            ret = this->config_apply(jo_input, &jo_out_error);
            if (!ret)
            {
                debug_log(L"pipe_thread config_apply error : %ws", to_wstring(jo_out_error).c_str());
            }
        }

        Json::Value jo_out;
        jo_out["succeed"] = ret;
        jo_out["error"] = jo_out_error;

        std::stringstream jo_out_buff_stream;
        jo_out_writer->write(jo_out, &jo_out_buff_stream);

        std::string jo_out_buff = jo_out_buff_stream.str();

        debug_log(L"pipe jo_out     : %ws", utf8_to_wstring(jo_out_buff).c_str());
        debug_log(L"pipe jo_out raw : %ws", to_hex_wstring((const uint8_t*)jo_out_buff.data(), jo_out_buff.size()).c_str());

        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ASYNC(WriteFile(this->ipc_pipe_hwnd, jo_out_buff.data(), static_cast<DWORD>(jo_out_buff.size()), NULL, &ol));

        FlushFileBuffers(this->ipc_pipe_hwnd);
        DisconnectNamedPipe(this->ipc_pipe_hwnd);
    }

    debug_log(L"void filter_core::pipe_thread: return.");
#pragma warning(default : 6387)
}
