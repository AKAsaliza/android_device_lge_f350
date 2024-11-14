LOCAL_PATH := $(call my-dir)

# Configuration scripts

include $(CLEAR_VARS)
LOCAL_MODULE            := set_baseband.sh
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := EXECUTABLES
LOCAL_SRC_FILES         := bin/set_baseband.sh
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR_EXECUTABLES)
include $(BUILD_PREBUILT)

# Init scripts

include $(CLEAR_VARS)
LOCAL_MODULE            := fstab.b1
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/fstab.b1
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR_ETC)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := init.b1.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/init.b1.rc
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR_ETC)/init/hw
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := init.b1.power.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/init.b1.power.rc
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR_ETC)/init/hw
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := init.b1.usb.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/init.b1.usb.rc
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR_ETC)/init/hw
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := init.recovery.b1.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/init.recovery.b1.rc
LOCAL_MODULE_PATH       := $(TARGET_RECOVERY_ROOT_OUT)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := init.recovery.b1.power.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/init.b1.power.rc
LOCAL_MODULE_PATH       := $(TARGET_RECOVERY_ROOT_OUT)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE            := ueventd.b1.rc
LOCAL_MODULE_STEM       := ueventd.rc
LOCAL_MODULE_TAGS       := optional
LOCAL_MODULE_CLASS      := ETC
LOCAL_SRC_FILES         := etc/ueventd.b1.rc
LOCAL_MODULE_PATH       := $(TARGET_OUT_VENDOR)
include $(BUILD_PREBUILT)
