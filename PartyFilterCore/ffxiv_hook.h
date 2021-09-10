#pragma once

typedef void(*receive_listing_delegate)(const void* manager, const void* data);

extern receive_listing_delegate receive_listing_original;

void receive_listing_new(const void* manager, const void* data);
