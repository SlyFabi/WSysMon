#include "../ServicesApi.h"

#include <systemd/sd-bus.h>

struct sd_unit_info {
    const char *id;
    const char *description;
    const char *load_state;
    const char *active_state;
    const char *sub_state;
    const char *following;
    const char *unit_path;
    uint32_t job_id;
    const char *job_type;
    const char *job_path;
};

inline int parse_unit(sd_bus_message *msg, sd_unit_info *u) {
    return sd_bus_message_read(msg, "(ssssssouso)", &u->id, &u->description, &u->load_state, &u->active_state, &u->sub_state, &u->following, &u->unit_path, &u->job_id, &u->job_type, &u->job_path);
}

std::vector<ServiceInfo> ServicesApi::GetAllServices() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = nullptr;
    sd_bus *bus = nullptr;
    sd_unit_info u{};
    std::vector<ServiceInfo> result{};

    int r = sd_bus_open_system(&bus);
    if (r < 0)
        goto end;

    r = sd_bus_call_method(bus,
                           "org.freedesktop.systemd1",
                           "/org/freedesktop/systemd1",
                           "org.freedesktop.systemd1.Manager",
                           "ListUnits",
                           &error,
                           &m,
                           "",
                           nullptr,
                           nullptr);
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
        goto end;
    }

    r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "(ssssssouso)");
    if (r < 0)
        goto end;

    while (parse_unit(m, &u) > 0) {
        if(!Utils::stringEndsWith(u.id, ".service"))
            continue;

        auto service = ServiceInfo();
        service.name = Utils::stringReplace(u.id, ".service", "");
        service.status = u.sub_state;
        result.emplace_back(service);

        //spdlog::debug("{}", u.sub_state);
    }

    end:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);
    return result;
}
