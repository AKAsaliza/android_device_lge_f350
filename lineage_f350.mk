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
    BUILD_FINGERPRINT=lge/b1_skt_kr/b1:5.0.1/LRX21Y/F350S20h.1464244928:user/release-keys \
    PRIVATE_BUILD_DESC="b1_skt_kr-user 5.0.1 LRX21Y F350S20h.1464244928 release-keys"
