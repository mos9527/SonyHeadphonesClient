// Host media control through MPRIS on the session bus, plus the local-adapter check the
// wearing-status "pause when removed" option needs. Plain libdbus, same as mdr-bt's Linux
// backend. Nothing here keeps state between calls; what was paused travels in the record
// handed back to the caller.
#include "../Platform.hpp"

#include <dbus/dbus.h>

#include <cstring>
#include <string>
#include <strings.h>
#include <vector>

struct ClientMediaPause
{
    std::vector<std::string> players;
};

namespace
{
    constexpr const char* kMprisPrefix = "org.mpris.MediaPlayer2.";
    constexpr const char* kMprisPath = "/org/mpris/MediaPlayer2";
    constexpr const char* kPlayerInterface = "org.mpris.MediaPlayer2.Player";
    constexpr int kTimeoutMs = 1000;

    DBusConnection* Bus(DBusBusType type)
    {
        DBusError error;
        dbus_error_init(&error);
        DBusConnection* bus = dbus_bus_get(type, &error);
        if (dbus_error_is_set(&error))
            dbus_error_free(&error);
        return bus;
    }

    DBusMessage* Call(DBusConnection* bus, DBusMessage* request)
    {
        if (!request)
            return nullptr;
        DBusMessage* reply = dbus_connection_send_with_reply_and_block(bus, request, kTimeoutMs, nullptr);
        dbus_message_unref(request);
        return reply;
    }

    std::vector<std::string> ListPlayers(DBusConnection* bus)
    {
        std::vector<std::string> players;
        DBusMessage* reply = Call(bus, dbus_message_new_method_call(
            "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames"));
        if (!reply)
            return players;
        DBusMessageIter iter, names;
        if (dbus_message_iter_init(reply, &iter) && dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY)
        {
            dbus_message_iter_recurse(&iter, &names);
            while (dbus_message_iter_get_arg_type(&names) == DBUS_TYPE_STRING)
            {
                const char* name = nullptr;
                dbus_message_iter_get_basic(&names, &name);
                if (name && std::string_view(name).starts_with(kMprisPrefix))
                    players.emplace_back(name);
                dbus_message_iter_next(&names);
            }
        }
        dbus_message_unref(reply);
        return players;
    }

    std::string PlaybackStatus(DBusConnection* bus, const std::string& player)
    {
        DBusMessage* request = dbus_message_new_method_call(
            player.c_str(), kMprisPath, "org.freedesktop.DBus.Properties", "Get");
        if (!request)
            return {};
        const char* property = "PlaybackStatus";
        dbus_message_append_args(request, DBUS_TYPE_STRING, &kPlayerInterface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);
        DBusMessage* reply = Call(bus, request);
        if (!reply)
            return {};
        std::string status;
        DBusMessageIter iter, variant;
        if (dbus_message_iter_init(reply, &iter) && dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT)
        {
            dbus_message_iter_recurse(&iter, &variant);
            if (dbus_message_iter_get_arg_type(&variant) == DBUS_TYPE_STRING)
            {
                const char* value = nullptr;
                dbus_message_iter_get_basic(&variant, &value);
                if (value)
                    status = value;
            }
        }
        dbus_message_unref(reply);
        return status;
    }

    void CallPlayer(DBusConnection* bus, const std::string& player, const char* method)
    {
        DBusMessage* reply = Call(bus, dbus_message_new_method_call(player.c_str(), kMprisPath, kPlayerInterface, method));
        if (reply)
            dbus_message_unref(reply);
    }

    // Addresses of every org.bluez.Adapter1 on the system bus, via ObjectManager.
    std::vector<std::string> AdapterAddresses(DBusConnection* bus)
    {
        std::vector<std::string> addresses;
        DBusMessage* reply = Call(bus, dbus_message_new_method_call(
            "org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects"));
        if (!reply)
            return addresses;
        DBusMessageIter iter, objects;
        if (!dbus_message_iter_init(reply, &iter) || dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY)
        {
            dbus_message_unref(reply);
            return addresses;
        }
        dbus_message_iter_recurse(&iter, &objects);
        for (; dbus_message_iter_get_arg_type(&objects) == DBUS_TYPE_DICT_ENTRY; dbus_message_iter_next(&objects))
        {
            DBusMessageIter object, interfaces;
            dbus_message_iter_recurse(&objects, &object);
            dbus_message_iter_next(&object); // skip the object path
            if (dbus_message_iter_get_arg_type(&object) != DBUS_TYPE_ARRAY)
                continue;
            dbus_message_iter_recurse(&object, &interfaces);
            for (; dbus_message_iter_get_arg_type(&interfaces) == DBUS_TYPE_DICT_ENTRY; dbus_message_iter_next(&interfaces))
            {
                DBusMessageIter interface, properties;
                dbus_message_iter_recurse(&interfaces, &interface);
                const char* name = nullptr;
                dbus_message_iter_get_basic(&interface, &name);
                if (!name || std::strcmp(name, "org.bluez.Adapter1") != 0)
                    continue;
                dbus_message_iter_next(&interface);
                if (dbus_message_iter_get_arg_type(&interface) != DBUS_TYPE_ARRAY)
                    continue;
                dbus_message_iter_recurse(&interface, &properties);
                for (; dbus_message_iter_get_arg_type(&properties) == DBUS_TYPE_DICT_ENTRY; dbus_message_iter_next(&properties))
                {
                    DBusMessageIter property, variant;
                    dbus_message_iter_recurse(&properties, &property);
                    const char* key = nullptr;
                    dbus_message_iter_get_basic(&property, &key);
                    if (!key || std::strcmp(key, "Address") != 0)
                        continue;
                    dbus_message_iter_next(&property);
                    dbus_message_iter_recurse(&property, &variant);
                    if (dbus_message_iter_get_arg_type(&variant) == DBUS_TYPE_STRING)
                    {
                        const char* value = nullptr;
                        dbus_message_iter_get_basic(&variant, &value);
                        if (value)
                            addresses.emplace_back(value);
                    }
                }
            }
        }
        dbus_message_unref(reply);
        return addresses;
    }
}

extern "C" {
int clientPlatformIsLocalBluetoothAddress(const char* address, int* outIsLocal)
{
    if (!address || !outIsLocal)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    DBusConnection* bus = Bus(DBUS_BUS_SYSTEM);
    if (!bus)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    *outIsLocal = 0;
    for (const std::string& adapter : AdapterAddresses(bus))
    {
        if (strcasecmp(adapter.c_str(), address) == 0)
        {
            *outIsLocal = 1;
            break;
        }
    }
    return MDR_RESULT_OK;
}

struct ClientMediaPause* clientPlatformMediaPause()
{
    DBusConnection* bus = Bus(DBUS_BUS_SESSION);
    if (!bus)
        return nullptr;
    auto* pause = new (std::nothrow) ClientMediaPause{};
    if (!pause)
        return nullptr;
    for (const std::string& player : ListPlayers(bus))
    {
        if (PlaybackStatus(bus, player) != "Playing")
            continue;
        CallPlayer(bus, player, "Pause");
        pause->players.push_back(player);
    }
    if (pause->players.empty())
    {
        delete pause;
        return nullptr;
    }
    return pause;
}

void clientPlatformMediaResume(struct ClientMediaPause* pause)
{
    if (!pause)
        return;
    if (DBusConnection* bus = Bus(DBUS_BUS_SESSION))
    {
        for (const std::string& player : pause->players)
        {
            if (PlaybackStatus(bus, player) == "Paused")
                CallPlayer(bus, player, "Play");
        }
    }
    delete pause;
}
}
