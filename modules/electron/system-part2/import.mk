SYSTEM_PART2_MODULE_VERSION ?= 1
SYSTEM_PART2_MODULE_PATH ?= $(PROJECT_ROOT)/modules/electron/system-part2
include $(call rwildcard,$(SYSTEM_PART2_MODULE_PATH)/,include.mk)
