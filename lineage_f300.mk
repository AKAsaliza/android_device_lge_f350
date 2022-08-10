# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/lge/f300/f300.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := f300
PRODUCT_NAME := lineage_f300
PRODUCT_BRAND := LGE
PRODUCT_MODEL := LG-F300
PRODUCT_MANUFACTURER := lge

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="g2_open_com-user 5.0.2 LRX22G 151061918340a release-keys"

BUILD_FINGERPRINT := lge/g2_open_com/g2:5.0.2/LRX22G/151061918340a:user/release-keys
