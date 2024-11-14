# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/lge/f350/f350.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := f350
PRODUCT_NAME := lineage_f350
PRODUCT_BRAND := LGE
PRODUCT_MODEL := LG-F350
PRODUCT_MANUFACTURER := lge

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="vu3_skt_kr-user 4.4.2 KOT49I.F300S20m F300S20m.1442799082 release-keys"

BUILD_FINGERPRINT := lge/vu3_skt_kr/vu3:4.4.2/KOT49I.F300S20m/F300S20m.1442799082:user/release-keys
