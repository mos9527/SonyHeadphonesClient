"""Enable/disable a Bluetooth service on a paired device via BluetoothSetServiceState (Windows only).

Lab tool for exposing the MDR RFCOMM service as a COM port, e.g. for Firefox's Web Serial,
which only sees OS-created serial ports. Enabling the service makes Windows create a
BTHENUM\\{service-uuid}_... devnode for the device; 'bind' then force-installs
"Standard Serial over Bluetooth link" (bthspp.inf) on it, the same as Device Manager's
Update driver -> Let me pick, to get a COM port. 'unbind' undoes that.

Usage:
    python tooling/scripts/bluetoothcom-win32.py list
    python tooling/scripts/bluetoothcom-win32.py services <device>
    python tooling/scripts/bluetoothcom-win32.py enable <device> [--uuid v2|v1|<uuid>]   (admin)
    python tooling/scripts/bluetoothcom-win32.py disable <device> [--uuid v2|v1|<uuid>]  (admin)
    python tooling/scripts/bluetoothcom-win32.py status <device> [--uuid ...]
    python tooling/scripts/bluetoothcom-win32.py bind <device> [--uuid ...]     (admin)
    python tooling/scripts/bluetoothcom-win32.py unbind <device> [--uuid ...]   (admin)

<device> is a substring of the device name, or its address (AA:BB:CC:DD:EE:FF).

This script is written by Claude Opus 5.5
"""

import argparse
import ctypes
import subprocess
import sys
import uuid
import winreg
from ctypes import wintypes

# Keep in sync with libmdr/include/mdr-c/Base.h
SERVICE_ALIASES = {
    "v2": "956C7B26-D49A-4BA8-B03F-B17D393CB6E2",
    "v1": "96CC203E-5068-46AD-B32D-E316F5E069BA",
    "spp": "00001101-0000-1000-8000-00805F9B34FB",
}

BLUETOOTH_SERVICE_DISABLE = 0x00
BLUETOOTH_SERVICE_ENABLE = 0x01


class GUID(ctypes.Structure):
    _fields_ = [
        ("Data1", wintypes.DWORD),
        ("Data2", wintypes.WORD),
        ("Data3", wintypes.WORD),
        ("Data4", ctypes.c_ubyte * 8),
    ]

    @classmethod
    def from_uuid(cls, u: uuid.UUID) -> "GUID":
        return cls.from_buffer_copy(u.bytes_le)

    def to_uuid(self) -> uuid.UUID:
        return uuid.UUID(bytes_le=bytes(self))


class SYSTEMTIME(ctypes.Structure):
    _fields_ = [(n, wintypes.WORD) for n in (
        "wYear", "wMonth", "wDayOfWeek", "wDay", "wHour", "wMinute", "wSecond", "wMilliseconds")]


class BLUETOOTH_DEVICE_INFO(ctypes.Structure):
    _fields_ = [
        ("dwSize", wintypes.DWORD),
        ("Address", ctypes.c_ulonglong),
        ("ulClassofDevice", wintypes.ULONG),
        ("fConnected", wintypes.BOOL),
        ("fRemembered", wintypes.BOOL),
        ("fAuthenticated", wintypes.BOOL),
        ("stLastSeen", SYSTEMTIME),
        ("stLastUsed", SYSTEMTIME),
        ("szName", wintypes.WCHAR * 248),
    ]

    @property
    def address_str(self) -> str:
        return ":".join(f"{(self.Address >> (8 * i)) & 0xFF:02X}" for i in reversed(range(6)))


class BLUETOOTH_DEVICE_SEARCH_PARAMS(ctypes.Structure):
    _fields_ = [
        ("dwSize", wintypes.DWORD),
        ("fReturnAuthenticated", wintypes.BOOL),
        ("fReturnRemembered", wintypes.BOOL),
        ("fReturnUnknown", wintypes.BOOL),
        ("fReturnConnected", wintypes.BOOL),
        ("fIssueInquiry", wintypes.BOOL),
        ("cTimeoutMultiplier", ctypes.c_ubyte),
        ("hRadio", wintypes.HANDLE),
    ]


class BLUETOOTH_FIND_RADIO_PARAMS(ctypes.Structure):
    _fields_ = [("dwSize", wintypes.DWORD)]


bt = ctypes.WinDLL("BluetoothApis.dll", use_last_error=True)
kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

bt.BluetoothFindFirstRadio.argtypes = [ctypes.POINTER(BLUETOOTH_FIND_RADIO_PARAMS), ctypes.POINTER(wintypes.HANDLE)]
bt.BluetoothFindFirstRadio.restype = wintypes.HANDLE
bt.BluetoothFindRadioClose.argtypes = [wintypes.HANDLE]
bt.BluetoothFindFirstDevice.argtypes = [ctypes.POINTER(BLUETOOTH_DEVICE_SEARCH_PARAMS), ctypes.POINTER(BLUETOOTH_DEVICE_INFO)]
bt.BluetoothFindFirstDevice.restype = wintypes.HANDLE
bt.BluetoothFindNextDevice.argtypes = [wintypes.HANDLE, ctypes.POINTER(BLUETOOTH_DEVICE_INFO)]
bt.BluetoothFindNextDevice.restype = wintypes.BOOL
bt.BluetoothFindDeviceClose.argtypes = [wintypes.HANDLE]
bt.BluetoothEnumerateInstalledServices.argtypes = [
    wintypes.HANDLE, ctypes.POINTER(BLUETOOTH_DEVICE_INFO), ctypes.POINTER(wintypes.DWORD), ctypes.POINTER(GUID)]
bt.BluetoothEnumerateInstalledServices.restype = wintypes.DWORD
bt.BluetoothSetServiceState.argtypes = [
    wintypes.HANDLE, ctypes.POINTER(BLUETOOTH_DEVICE_INFO), ctypes.POINTER(GUID), wintypes.DWORD]
bt.BluetoothSetServiceState.restype = wintypes.DWORD
kernel32.CloseHandle.argtypes = [wintypes.HANDLE]

ERROR_MORE_DATA = 234


# SetupAPI's own codes (0xE00002xx) aren't known to FormatMessage.
SETUPAPI_ERRORS = {
    0xE0000203: "ERROR_NO_DRIVER_SELECTED",
    0xE0000209: "ERROR_NO_ASSOCIATED_CLASS",
    0xE000020B: "ERROR_NO_SUCH_DEVINST",
    0xE0000219: "ERROR_NO_ASSOCIATED_SERVICE",
    0xE0000228: "ERROR_DI_DONT_INSTALL",
    0xE0000230: "ERROR_DRIVER_STORE_ADD_FAILED",
    0xE0000234: "ERROR_DEVICE_INSTALL_BLOCKED",
    0xE0000247: "ERROR_DRIVER_INSTALL_BLOCKED",
    0xE0000248: "ERROR_DRIVER_NOT_FOUND",
}


def fail(msg: str, code: int | None = None) -> None:
    if code is not None:
        u = code & 0xFFFFFFFF
        desc = SETUPAPI_ERRORS.get(u) or ctypes.FormatError(code).strip()
        msg += f": [0x{u:08X}] {desc}"
    sys.exit(msg)


def first_radio() -> wintypes.HANDLE:
    params = BLUETOOTH_FIND_RADIO_PARAMS(ctypes.sizeof(BLUETOOTH_FIND_RADIO_PARAMS))
    radio = wintypes.HANDLE()
    find = bt.BluetoothFindFirstRadio(ctypes.byref(params), ctypes.byref(radio))
    if not find:
        fail("No Bluetooth radio found", ctypes.get_last_error())
    bt.BluetoothFindRadioClose(find)
    return radio


def paired_devices(radio) -> list[BLUETOOTH_DEVICE_INFO]:
    params = BLUETOOTH_DEVICE_SEARCH_PARAMS(
        dwSize=ctypes.sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
        fReturnAuthenticated=True, fReturnRemembered=True, fReturnConnected=True,
        fReturnUnknown=False, fIssueInquiry=False, cTimeoutMultiplier=0, hRadio=radio)
    devices = []
    info = BLUETOOTH_DEVICE_INFO(dwSize=ctypes.sizeof(BLUETOOTH_DEVICE_INFO))
    find = bt.BluetoothFindFirstDevice(ctypes.byref(params), ctypes.byref(info))
    if not find:
        return devices
    try:
        while True:
            devices.append(info)
            info = BLUETOOTH_DEVICE_INFO(dwSize=ctypes.sizeof(BLUETOOTH_DEVICE_INFO))
            if not bt.BluetoothFindNextDevice(find, ctypes.byref(info)):
                break
    finally:
        bt.BluetoothFindDeviceClose(find)
    return devices


def pick_device(radio, query: str) -> BLUETOOTH_DEVICE_INFO:
    devices = paired_devices(radio)
    q = query.upper().replace("-", ":")
    matches = [d for d in devices if d.address_str == q] or \
              [d for d in devices if query.lower() in d.szName.lower()]
    if not matches:
        fail(f"No paired device matches {query!r}. Run 'list' to see paired devices.")
    if len(matches) > 1:
        names = ", ".join(f"{d.szName} ({d.address_str})" for d in matches)
        fail(f"{query!r} is ambiguous: {names}")
    return matches[0]


def installed_services(radio, dev: BLUETOOTH_DEVICE_INFO) -> list[uuid.UUID]:
    count = wintypes.DWORD(0)
    err = bt.BluetoothEnumerateInstalledServices(radio, ctypes.byref(dev), ctypes.byref(count), None)
    if err not in (0, ERROR_MORE_DATA):
        fail("BluetoothEnumerateInstalledServices failed", err)
    if count.value == 0:
        return []
    guids = (GUID * count.value)()
    err = bt.BluetoothEnumerateInstalledServices(radio, ctypes.byref(dev), ctypes.byref(count), guids)
    if err:
        fail("BluetoothEnumerateInstalledServices failed", err)
    return [g.to_uuid() for g in guids[: count.value]]


def resolve_uuid(s: str) -> uuid.UUID:
    return uuid.UUID(SERVICE_ALIASES.get(s.lower(), s))


def label(u: uuid.UUID) -> str:
    for name, val in SERVICE_ALIASES.items():
        if uuid.UUID(val) == u:
            return f"{u}  ({name.upper()})"
    return str(u)


def serial_ports() -> list[tuple[str, str]]:
    ports = []
    try:
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DEVICEMAP\SERIALCOMM") as key:
            i = 0
            while True:
                try:
                    name, value, _ = winreg.EnumValue(key, i)
                except OSError:
                    break
                ports.append((value, name))
                i += 1
    except FileNotFoundError:
        pass
    return ports


def bthenum_nodes(service: uuid.UUID, dev: BLUETOOTH_DEVICE_INFO) -> list[str]:
    """Return pnputil's blocks for BTHENUM devnodes of this service+device."""
    try:
        out = subprocess.run(["pnputil", "/enum-devices", "/bus", "BTHENUM"],
                             capture_output=True, text=True, errors="replace").stdout
    except FileNotFoundError:
        return ["(pnputil unavailable)"]
    needle_svc = "{" + str(service).lower() + "}"
    needle_addr = f"{dev.Address:012x}"
    blocks = [b.strip() for b in out.replace("\r", "").split("\n\n")]
    return [b for b in blocks if needle_svc in b.lower() and needle_addr in b.lower()]


def cmd_list(radio, _args) -> None:
    devices = paired_devices(radio)
    if not devices:
        print("No paired devices.")
    for d in devices:
        flags = ",".join(f for f, on in (("connected", d.fConnected), ("paired", d.fAuthenticated)) if on)
        print(f"{d.address_str}  {d.szName}  [{flags}]")


def cmd_services(radio, args) -> None:
    dev = pick_device(radio, args.device)
    print(f"Installed (enabled) services on {dev.szName} ({dev.address_str}):")
    for u in installed_services(radio, dev):
        print(f"  {label(u)}")


def cmd_status(radio, args) -> None:
    dev = pick_device(radio, args.device)
    svc = resolve_uuid(args.uuid)
    enabled = svc in installed_services(radio, dev)
    print(f"{dev.szName} ({dev.address_str}) service {label(svc)}: {'ENABLED' if enabled else 'not enabled'}")
    nodes = bthenum_nodes(svc, dev)
    print(f"\nBTHENUM devnodes ({len(nodes)}):")
    for n in nodes:
        print("  " + n.replace("\n", "\n  "))
    print("\nSerial ports (HKLM\\HARDWARE\\DEVICEMAP\\SERIALCOMM):")
    for port, driver in serial_ports() or [("(none)", "")]:
        print(f"  {port:8} {driver}")


def require_admin(what: str) -> None:
    if not ctypes.windll.shell32.IsUserAnAdmin():
        fail(f"{what} requires an elevated (Administrator) prompt.")


def cmd_set(radio, args, enable: bool) -> None:
    dev = pick_device(radio, args.device)
    svc = resolve_uuid(args.uuid)
    if (svc in installed_services(radio, dev)) == enable:
        print(f"{label(svc)} is already {'enabled' if enable else 'disabled'} on {dev.szName}; nothing to do.")
        return
    require_admin("BluetoothSetServiceState")
    guid = GUID.from_uuid(svc)
    flags = BLUETOOTH_SERVICE_ENABLE if enable else BLUETOOTH_SERVICE_DISABLE
    err = bt.BluetoothSetServiceState(radio, ctypes.byref(dev), ctypes.byref(guid), flags)
    if err:
        fail(f"BluetoothSetServiceState({label(svc)}, {'ENABLE' if enable else 'DISABLE'}) failed", err)
    print(f"{'Enabled' if enable else 'Disabled'} {label(svc)} on {dev.szName} ({dev.address_str}).")
    if enable:
        print("\nNext: run 'bind' to install the serial driver on the new devnode.")


# --- Driver binding (the Device Manager "Let me pick" step, via SetupAPI/newdev) ---

class SP_DEVINFO_DATA(ctypes.Structure):
    _fields_ = [("cbSize", wintypes.DWORD), ("ClassGuid", GUID),
                ("DevInst", wintypes.DWORD), ("Reserved", ctypes.c_size_t)]


class SP_DEVINSTALL_PARAMS_W(ctypes.Structure):
    _fields_ = [("cbSize", wintypes.DWORD), ("Flags", wintypes.DWORD), ("FlagsEx", wintypes.DWORD),
                ("hwndParent", wintypes.HWND), ("InstallMsgHandler", ctypes.c_void_p),
                ("InstallMsgHandlerContext", ctypes.c_void_p), ("FileQueue", ctypes.c_void_p),
                ("ClassInstallReserved", ctypes.c_size_t), ("Reserved", wintypes.DWORD),
                ("DriverPath", wintypes.WCHAR * 260)]


class SP_DRVINFO_DATA_V2_W(ctypes.Structure):
    _fields_ = [("cbSize", wintypes.DWORD), ("DriverType", wintypes.DWORD), ("Reserved", ctypes.c_size_t),
                ("Description", wintypes.WCHAR * 256), ("MfgName", wintypes.WCHAR * 256),
                ("ProviderName", wintypes.WCHAR * 256), ("DriverDate", wintypes.FILETIME),
                ("DriverVersion", ctypes.c_ulonglong)]


DI_ENUMSINGLEINF = 0x00010000
DI_FLAGSEX_ALLOWEXCLUDEDDRVS = 0x00000800
SPDIT_CLASSDRIVER = 1
SPDRP_CLASSGUID = 0x08
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value
GUID_DEVCLASS_PORTS = "{4D36E978-E325-11CE-BFC1-08002BE10318}"
BTHSPP_INF = r"C:\Windows\INF\bthspp.inf"
BTHSPP_DESC = "Standard Serial over Bluetooth link"

setupapi = ctypes.WinDLL("setupapi", use_last_error=True)
newdev = ctypes.WinDLL("newdev", use_last_error=True)
setupapi.SetupDiCreateDeviceInfoList.argtypes = [ctypes.c_void_p, wintypes.HWND]
setupapi.SetupDiCreateDeviceInfoList.restype = ctypes.c_void_p
setupapi.SetupDiDestroyDeviceInfoList.argtypes = [ctypes.c_void_p]
setupapi.SetupDiOpenDeviceInfoW.argtypes = [
    ctypes.c_void_p, wintypes.LPCWSTR, wintypes.HWND, wintypes.DWORD, ctypes.POINTER(SP_DEVINFO_DATA)]
setupapi.SetupDiOpenDeviceInfoW.restype = wintypes.BOOL
setupapi.SetupDiSetDeviceRegistryPropertyW.argtypes = [
    ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), wintypes.DWORD, ctypes.c_void_p, wintypes.DWORD]
setupapi.SetupDiSetDeviceRegistryPropertyW.restype = wintypes.BOOL
for fn in ("SetupDiGetDeviceInstallParamsW", "SetupDiSetDeviceInstallParamsW"):
    getattr(setupapi, fn).argtypes = [
        ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), ctypes.POINTER(SP_DEVINSTALL_PARAMS_W)]
    getattr(setupapi, fn).restype = wintypes.BOOL
setupapi.SetupDiBuildDriverInfoList.argtypes = [ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), wintypes.DWORD]
setupapi.SetupDiBuildDriverInfoList.restype = wintypes.BOOL
setupapi.SetupDiEnumDriverInfoW.argtypes = [
    ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), wintypes.DWORD, wintypes.DWORD,
    ctypes.POINTER(SP_DRVINFO_DATA_V2_W)]
setupapi.SetupDiEnumDriverInfoW.restype = wintypes.BOOL
setupapi.SetupDiSetSelectedDriverW.argtypes = [
    ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), ctypes.POINTER(SP_DRVINFO_DATA_V2_W)]
setupapi.SetupDiSetSelectedDriverW.restype = wintypes.BOOL
newdev.DiInstallDevice.argtypes = [
    wintypes.HWND, ctypes.c_void_p, ctypes.POINTER(SP_DEVINFO_DATA), ctypes.POINTER(SP_DRVINFO_DATA_V2_W),
    wintypes.DWORD, ctypes.POINTER(wintypes.BOOL)]
newdev.DiInstallDevice.restype = wintypes.BOOL


def check(ok, what: str) -> None:
    if not ok:
        fail(f"{what} failed", ctypes.get_last_error())


def instance_id(svc: uuid.UUID, dev: BLUETOOTH_DEVICE_INFO) -> str:
    nodes = bthenum_nodes(svc, dev)
    if not nodes:
        fail(f"No BTHENUM devnode for {label(svc)} on {dev.szName}. Run 'enable' first.")
    for line in nodes[0].splitlines():
        if line.startswith("Instance ID:"):
            return line.split(":", 1)[1].strip()
    fail("Could not parse the devnode's instance ID from pnputil output.")


def cmd_bind(radio, args) -> None:
    if ctypes.sizeof(ctypes.c_void_p) != 8:
        fail("bind needs 64-bit Python (SetupAPI struct packing differs on 32-bit).")
    dev = pick_device(radio, args.device)
    svc = resolve_uuid(args.uuid)
    inst = instance_id(svc, dev)
    require_admin("Installing a driver")
    print(f"Binding '{BTHSPP_DESC}' to\n  {inst}")

    hset = setupapi.SetupDiCreateDeviceInfoList(None, None)
    check(hset and hset != INVALID_HANDLE_VALUE, "SetupDiCreateDeviceInfoList")
    try:
        data = SP_DEVINFO_DATA(cbSize=ctypes.sizeof(SP_DEVINFO_DATA))
        check(setupapi.SetupDiOpenDeviceInfoW(hset, inst, None, 0, ctypes.byref(data)), "SetupDiOpenDeviceInfo")

        # The devnode has no class yet; class drivers are listed per class, so move it to Ports first.
        cls = ctypes.create_unicode_buffer(GUID_DEVCLASS_PORTS)
        check(setupapi.SetupDiSetDeviceRegistryPropertyW(
            hset, ctypes.byref(data), SPDRP_CLASSGUID, cls, ctypes.sizeof(cls)), "Setting class to Ports")

        # Restrict the driver search to bthspp.inf, and allow models that don't list our hardware ID.
        params = SP_DEVINSTALL_PARAMS_W(cbSize=ctypes.sizeof(SP_DEVINSTALL_PARAMS_W))
        check(setupapi.SetupDiGetDeviceInstallParamsW(hset, ctypes.byref(data), ctypes.byref(params)),
              "SetupDiGetDeviceInstallParams")
        params.Flags |= DI_ENUMSINGLEINF
        params.FlagsEx |= DI_FLAGSEX_ALLOWEXCLUDEDDRVS
        params.DriverPath = BTHSPP_INF
        check(setupapi.SetupDiSetDeviceInstallParamsW(hset, ctypes.byref(data), ctypes.byref(params)),
              "SetupDiSetDeviceInstallParams")
        check(setupapi.SetupDiBuildDriverInfoList(hset, ctypes.byref(data), SPDIT_CLASSDRIVER),
              "SetupDiBuildDriverInfoList")

        drv = SP_DRVINFO_DATA_V2_W(cbSize=ctypes.sizeof(SP_DRVINFO_DATA_V2_W))
        i, found = 0, False
        while setupapi.SetupDiEnumDriverInfoW(hset, ctypes.byref(data), SPDIT_CLASSDRIVER, i, ctypes.byref(drv)):
            if drv.Description == BTHSPP_DESC:
                found = True
                break
            i += 1
        if not found:
            fail(f"'{BTHSPP_DESC}' not found in {BTHSPP_INF}")
        check(setupapi.SetupDiSetSelectedDriverW(hset, ctypes.byref(data), ctypes.byref(drv)),
              "SetupDiSetSelectedDriver")

        # Pass the driver explicitly: with NULL, DiInstallDevice ignores the selection and searches
        # the driver store for a hardware-ID match, which fails with ERROR_NO_DRIVER_SELECTED.
        reboot = wintypes.BOOL(False)
        check(newdev.DiInstallDevice(None, hset, ctypes.byref(data), ctypes.byref(drv), 0, ctypes.byref(reboot)),
              "DiInstallDevice")
    finally:
        setupapi.SetupDiDestroyDeviceInfoList(hset)

    print("Driver installed." + (" A reboot is required." if reboot.value else ""))
    print("\nSerial ports now:")
    for port, driver in serial_ports() or [("(none)", "")]:
        print(f"  {port:8} {driver}")


def cmd_unbind(radio, args) -> None:
    dev = pick_device(radio, args.device)
    svc = resolve_uuid(args.uuid)
    inst = instance_id(svc, dev)
    require_admin("Removing a device")
    # Removing the devnode drops the forced driver; a rescan re-creates it driverless.
    for cmd in (["pnputil", "/remove-device", inst], ["pnputil", "/scan-devices"]):
        r = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
        print(r.stdout.strip())
        if r.returncode:
            fail(f"{' '.join(cmd[:2])} failed ({r.returncode}): {r.stderr.strip()}")


def main() -> None:
    if sys.platform != "win32":
        sys.exit("Windows only.")
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    sub.add_parser("list", help="list paired devices")
    for name, hlp in (("services", "list enabled services of a device"),
                      ("status", "show service state, BTHENUM devnodes and COM ports"),
                      ("enable", "enable a service (admin)"),
                      ("disable", "disable a service (admin)"),
                      ("bind", "force-install 'Standard Serial over Bluetooth link' on the service devnode (admin)"),
                      ("unbind", "remove the devnode so it comes back driverless (admin)")):
        sp = sub.add_parser(name, help=hlp)
        sp.add_argument("device", help="name substring or address")
        if name != "services":
            sp.add_argument("--uuid", default="v2", help="service UUID or alias: v2 (default), v1, spp")
    args = p.parse_args()

    radio = first_radio()
    try:
        {"list": cmd_list, "services": cmd_services, "status": cmd_status,
         "enable": lambda r, a: cmd_set(r, a, True),
         "disable": lambda r, a: cmd_set(r, a, False),
         "bind": cmd_bind, "unbind": cmd_unbind}[args.cmd](radio, args)
    finally:
        kernel32.CloseHandle(radio)


if __name__ == "__main__":
    main()
