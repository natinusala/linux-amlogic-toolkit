# Now used only by Trusty
LOCAL_ADDITIONAL_DEPENDENCIES += $(LOCAL_PATH)/sources.mk
include $(LOCAL_PATH)/sources.mk

LOCAL_CFLAGS += -I$(LOCAL_PATH)/src/include -I$(LOCAL_PATH)/src/crypto -Wno-unused-parameter -DBORINGSSL_ANDROID_SYSTEM
LOCAL_ASFLAGS += -I$(LOCAL_PATH)/src/include -I$(LOCAL_PATH)/src/crypto -Wno-unused-parameter
# Do not add in the architecture-specific files if we don't want to build assembly
LOCAL_SRC_FILES_$(ARCH) := $(linux_$(STANDARD_ARCH_NAME)_sources)
LOCAL_SRC_FILES += $(crypto_sources)
