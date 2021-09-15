#include "javascript_engine.h"

#include <exception>
#include <sstream>

#include <Windows.h>

#include <duktape.h>

#include "defer.h"
#include "utils.h"
#include "debug_log.h"

#define push_object_global(idx_name, name ) const auto idx_name = duk_push_object(ctx); defer(duk_put_global_string(ctx, name));

#define push_object(    idx_name, idx_parent, name ) const auto idx_name = duk_push_object(ctx); defer(duk_put_prop_string(ctx, idx_parent, name));
#define push_array(     idx_name, idx_parent, name ) const auto idx_name = duk_push_array (ctx); defer(duk_put_prop_string(ctx, idx_parent, name));
#define push_array_item(idx_name, idx_parent, index) const auto idx_name = duk_push_object(ctx); defer(duk_put_prop_index (ctx, idx_parent, index));

#define push_string(idx, name, value) { duk_push_string (ctx, (const char*)value);       duk_put_prop_string(ctx, idx, name); }
#define push_int(   idx, name, value) { duk_push_int    (ctx, (duk_int_t  )value);       duk_put_prop_string(ctx, idx, name); }
#define push_bool(  idx, name, value) { duk_push_boolean(ctx,              value);       duk_put_prop_string(ctx, idx, name); }
#define push_null(  idx, name       ) { duk_push_null   (ctx                    );       duk_put_prop_string(ctx, idx, name); }

static duk_ret_t custom_print(duk_context* ctx)
{
    duk_idx_t nargs;
    const duk_uint8_t* buf;
    duk_size_t sz_buf;
    const char nl = '\n';
    duk_uint8_t buf_stack[256]{};

    nargs = duk_get_top(ctx);

    if (nargs == 1 && duk_is_buffer_data(ctx, 0))
    {
        buf = (const duk_uint8_t*)duk_get_buffer_data(ctx, 0, &sz_buf);
    }
    else if (nargs > 0)
    {
        duk_idx_t i;
        duk_size_t sz_str;
        const duk_uint8_t* p_str;
        duk_uint8_t* p;

        sz_buf = (duk_size_t)nargs;  /* spaces (nargs - 1) + newline */
        for (i = 0; i < nargs; i++)
        {
            (void)duk_to_lstring(ctx, i, &sz_str);
            sz_buf += sz_str;
        }

        if (sz_buf <= sizeof(buf_stack))
        {
            p = (duk_uint8_t*)buf_stack;
        }
        else
        {
            p = (duk_uint8_t*)duk_push_fixed_buffer(ctx, sz_buf + 1);
        }

        buf = (const duk_uint8_t*)p;
        for (i = 0; i < nargs; i++)
        {
            p_str = (const duk_uint8_t*)duk_get_lstring(ctx, i, &sz_str);
            memcpy((void*)p, (const void*)p_str, sz_str);
            p += sz_str;
            *p++ = (duk_uint8_t)(i == nargs - 1 ? '\n' : ' ');
        }
        *p++ = 0;
    }
    else
    {
        buf = (const duk_uint8_t*)&nl;
        sz_buf = 1;
    }

    if (sz_buf > 0)
    {
        debug_log(L"javascript print: %ws", utf8_to_wstring((const char*)buf).c_str());
    }

    return 0;
}

javascript_engine::javascript_engine()
{
    auto ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr, nullptr);

    duk_push_global_object(ctx);
    duk_push_string(ctx, "print");
    duk_push_c_function(ctx, custom_print, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    duk_pop(ctx);

    {
        push_object_global(idx_job, "jobs");
        for (auto& v : ffxiv_job_id)
        {
            push_int(idx_job, v.first, v.second);
        }
    }

    {
        push_object_global(idx_slot_flags, "slot_flags");
        for (auto& v : ffxiv_slot_flag)
        {
            push_int(idx_slot_flags, v.first, v.second);
        }
    }

    {
        push_object_global(idx_content_ids, "content_ids");
        for (auto& v : ffxiv_content_id)
        {
            push_int(idx_content_ids, v.first, v.second);
        }
    }

    {
        push_object_global(idx_item_rules, "item_rules");
        push_int(idx_item_rules, "normal", 0);
        push_int(idx_item_rules, "greed", 1);
        push_int(idx_item_rules, "master", 2);
    }

    {
        push_object_global(idx_objectives, "objectives");
        push_int(idx_objectives, "none", 0);
        push_int(idx_objectives, "duty_completion", 1);
        push_int(idx_objectives, "practice", 2);
        push_int(idx_objectives, "loot", 4);
    }

    this->ctx = ctx;
}

javascript_engine::~javascript_engine()
{
    auto ctx = (duk_context*)this->ctx;
    duk_destroy_heap(ctx);
}

bool javascript_engine::eval(const party_finder_packet_item* item, const std::string& script, bool* succeed, std::string* error_message)
{
    /*
    var party = {
        // 모집자 이름
        "name": "@@@@@@@@@",

        // 모집자 설명
        "description": "@@@@@@@@@",

        // 모집 남은 시간
        "remaining_seconds": 0,

        // 임무명
        "content_id": 0,

        // 무작위 임무인가?
        "is_roulette": 0,

        // 0
        "content_category": 0,

        // 파티 수
        "party_count": 1,

        // 비공개 파티
        "is_private" : true,

        // 24 인 파티
        "is_24" : true,

        // 같은 서버
        "same_server" : true,

        // 잡 중복 없음
        "no_dup_job" : true

        // 초보자 환영
        "welcome_beginner": false

        // 최소 아이템 레벨
        "minimum_level": 0

        // 인원수 제한 해제
        "undersize_player": false,

        // 초월하는 힘
        "silence_echo": false

        // 0기본 1선입찰금지 2파티장분배
        "item_rule": 0,

        // 목적 0설정안함 / 1: 완료 목적 / 2: 연습 / 3: 반복 공략
        "objective": 0,

        // 남은 자리
        "slots": 1,

        // 모집 슬롯
        "slot": [
            {
                // 지금 자리에 있는 직업...
                "in_slot": 0,

                // 모집중인 직업 갯수... 예) 탱커 -> 나 전 암 건 + 검술사 도끼술사 해서 6개...
                "available_jobs" : 1,
            },
            ...
        ]
    };
    */

    auto ctx = (duk_context*)this->ctx;
    {
        push_object_global(idx_value, "party");
        {
            push_string(idx_value, "owner_name", item->owner_name.data());
            push_string(idx_value, "description", item->desc.data());

            push_int(idx_value, "remaining_seconds", item->remaining_seconds);

            push_int(idx_value, "content_category", item->content_category);

            push_int( idx_value, "content_id", item->content_id);
            push_bool(idx_value, "is_eden", is_eden_raid(item->content_id));

            push_bool(idx_value, "is_roulette", item->is_roulette_eq_1 == 1);

            push_int(idx_value, "party_count", item->party_count);

            const auto is_24 = (item->settings2 & (1 << 2)) != 0;
            push_bool(idx_value, "is_private" , (item->settings2 & (1 << 1)) == 0);
            push_bool(idx_value, "is_24"      , is_24);
            push_bool(idx_value, "same_server", (item->settings2 & (1 << 3)) != 0);
            push_bool(idx_value, "no_dup_job" , (item->settings2 & (1 << 5)) != 0);

            push_bool(idx_value, "welcome_beginner", item->welcome_beginner == 1);

            push_bool(idx_value, "undersize_player", ((item->settings & (1 << 0)) != 0));
            push_int( idx_value, "minimum_level", ((item->settings & (1 << 1)) == 0) ? 0 : item->min_item_level);
            push_bool(idx_value, "silence_echo", ((item->settings & (1 << 2)) != 0));

            push_int(idx_value, "item_rule", item->item_rule);

            push_int(idx_value, "slot_count", item->slot_count);

            push_int(idx_value, "objective", item->objective);

            {
                push_array(idx_array, idx_value, "slot");

                if (!is_24)
                {
                    for (size_t i = 0; i < item->slot_count && i < item->slot_user_job.size(); i++)
                    {
                        push_array_item(idx_array_item, idx_array, i);

                        const auto is_empty = item->slot_user_job[i] == 0;

                        int available_jobs = 0;
                        for (auto& v : ffxiv_slot_flag)
                        {
                            if (is_empty && (item->slot_available_flag[i] & v.second) != 0)
                            {
                                if (v.first[0] == '_')
                                {
                                    push_bool(idx_array_item, v.first + 1, true);
                                }
                                else
                                {
                                    push_bool(idx_array_item, v.first, true);
                                    available_jobs++;
                                }
                            }
                            else
                            {
                                push_bool(idx_array_item, v.first, false);
                            }
                        }

                        push_int(idx_array_item, "in_slot", is_empty ? 0 : item->slot_user_job[i]);
                        push_int(idx_array_item, "_raw", is_empty ? 0 : item->slot_available_flag[i]);
                        push_int(idx_array_item, "available_jobs", is_empty ? available_jobs : 0);
                    }
                }
            }
        }
    };

    /*
    __try
    {
    */
    try
    {
        duk_eval_string(ctx, script.c_str());
    }
    catch (const std::runtime_error& ex)
    {
        debug_log(L"duk_eval_string exception : std::runtime_error");
        if (succeed != nullptr) *succeed = false;
        if (error_message != nullptr) *error_message = ex.what();

        return false;
    }
    catch (const std::exception& ex)
    {
        debug_log(L"duk_eval_string exception : std::exception");
        if (succeed != nullptr) *succeed = false;
        if (error_message != nullptr) *error_message = ex.what();

        return false;
    }
    catch (...)
    {
        debug_log(L"duk_eval_string exception : ...");
        if (succeed != nullptr) *succeed = false;
        if (error_message != nullptr) *error_message = "unknown exception.";

        return false;
    }
    /*
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        debug_log(L"duk_eval_string exception : EXCEPTION_EXECUTE_HANDLER");

        auto ex = GetExceptionInformation();

        if (succeed != nullptr) *succeed = false;
        if (error_message != nullptr)
        {
            std::stringstream ss;
            ss << "unknown exception ExceptionCode: " << ex->ExceptionRecord->ExceptionCode << " ExceptionAddress : " << std::hex << ex->ExceptionRecord->ExceptionAddress;

            *error_message = ss.str();
        }

        return false;
    }
    */



    defer(duk_pop(ctx));

    if (duk_is_boolean(ctx, -1) == false)
    {
        if (succeed != nullptr) *succeed = false;

        if (error_message != nullptr)
        {
            std::string type_name;
            switch (duk_get_type(ctx, -1))
            {
            case DUK_TYPE_NUMBER:    type_name = "number"          ; break;
            case DUK_TYPE_UNDEFINED: type_name = "undefined"       ; break;
            case DUK_TYPE_NULL:      type_name = "null"            ; break;
            case DUK_TYPE_STRING:    type_name = "string"          ; break;
            case DUK_TYPE_OBJECT:    type_name = "object"          ; break;
            case DUK_TYPE_BUFFER:    type_name = "buffer"          ; break;
            case DUK_TYPE_POINTER:   type_name = "raw void pointer"; break;
            default:                 type_name = "unknown"         ; break;
            }

            *error_message = "Returns must be boolean. type = " + type_name;
        }

        return false;
    }

    return duk_get_boolean(ctx, -1);
}
