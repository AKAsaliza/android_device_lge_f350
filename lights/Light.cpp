/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "light"

#include "Light.h"

#include <thread>
#include <chrono>

#include <log/log.h>

#define LCD_BRIGHTNESS_MIN 20 // Matches config_screenBrightnessSettingMinimum
#define LCD_BRIGHTNESS_MAX 255
#define LCD_BRIGHTNESS_DELTA (LCD_BRIGHTNESS_MAX - LCD_BRIGHTNESS_MIN)

namespace {
using android::hardware::light::V2_0::LightState;

static uint32_t rgbToBrightness(const LightState& state) {
    uint32_t color = state.color & 0x00ffffff;
    return ((77 * ((color >> 16) & 0xff)) + (150 * ((color >> 8) & 0xff)) +
            (29 * (color & 0xff))) >> 8;
}

static bool isLit(const LightState& state) {
    return (state.color & 0x00ffffff);
}

static uint32_t applyGamma(const uint32_t brightness){
    if(brightness < LCD_BRIGHTNESS_MIN)
        return 0;

    return LCD_BRIGHTNESS_MIN + LCD_BRIGHTNESS_DELTA *
        cbrt(((double)brightness - LCD_BRIGHTNESS_MIN)/LCD_BRIGHTNESS_DELTA);
}
} // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Light::Light(std::ofstream&& backlight, std::ofstream&& blinkPattern,
             std::ofstream&& rearRed,
             std::ofstream&& rearGreen,
             std::ofstream&& rearBlue,
			 std::ofstream&& rearEnable) :
    mBacklight(std::move(backlight)), 
	mBlinkPattern(std::move(blinkPattern)),
    mRearRed(std::move(rearRed)), 
	mRearGreen(std::move(rearGreen)),
    mRearBlue(std::move(rearBlue)), 
	mRearEnable(std::move(rearEnable)),
    mScreenOn(false) {	
    auto attnFn(std::bind(&Light::setAttentionLight, this, std::placeholders::_1));
    auto backlightFn(std::bind(&Light::setBacklight, this, std::placeholders::_1));
    auto batteryFn(std::bind(&Light::setBatteryLight, this, std::placeholders::_1));
    auto notifFn(std::bind(&Light::setNotificationLight, this, std::placeholders::_1));
    mLights.emplace(std::make_pair(Type::ATTENTION, attnFn));
    mLights.emplace(std::make_pair(Type::BACKLIGHT, backlightFn));
    mLights.emplace(std::make_pair(Type::BATTERY, batteryFn));
    mLights.emplace(std::make_pair(Type::NOTIFICATIONS, notifFn));
}

// Methods from ::android::hardware::light::V2_0::ILight follow.
Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = mLights.find(type);
    if (it == mLights.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }
    it->second(state);
    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;
    for (auto const& light : mLights) {
        types.push_back(light.first);
    }
    _hidl_cb(types);
    return Void();
}

void Light::setAttentionLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mAttentionState = state;
    setSpeakerBatteryLightLocked();
}

void Light::setBacklight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    uint32_t brightness = rgbToBrightness(state);

	brightness = applyGamma(brightness);
	bool wasScreenOn = mScreenOn;
    mScreenOn = ((state.color & 0x00ffffff) > 0);
    
    if (!wasScreenOn && mScreenOn) {
        if ((mBatteryState.color & 0x00ffffff) == 0) {
            mBlinkPattern << "0x0,-1,-1" << std::endl;
        }

        // Rear RGB Screen ON CYAN
        if (!mFadeActive) {
            std::thread([this]() {
                mFadeActive = true;
				
				int i = 255;
                mRearEnable.clear(); mRearEnable << 1 << std::endl;
                    mRearRed << 0 << std::endl;
                    mRearGreen << i << std::endl; // Green
                    mRearBlue << i << std::endl;  // Blue (G+B = Cyan)
				std::this_thread::sleep_for(std::chrono::milliseconds(300));

                for (int i = 255; i >= 0; i -= 15) {
                    mRearRed.clear(); mRearGreen.clear(); mRearBlue.clear();
                    mRearRed << 0 << std::endl;
                    mRearGreen << i << std::endl; // Green
                    mRearBlue << i << std::endl;  // Blue (G+B = Cyan)
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
                
                mFadeActive = false;

                std::lock_guard<std::mutex> lck(mLock);
                setRearBatteryLightLocked(mBatteryState); 
            }).detach();
        }
    }
	
	else if (wasScreenOn && !mScreenOn) {
        setRearBatteryLightLocked(mBatteryState);
    }
	
    // Prevent backlight from being turned off completely
    if(brightness == 0 && mScreenOn) {
        brightness = LCD_BRIGHTNESS_MIN;
    }
	
   mBacklight << brightness << std::endl;

}

void Light::setBatteryLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mBatteryState = state;
    setSpeakerBatteryLightLocked();
	setRearBatteryLightLocked(state);
}

void Light::setRearBatteryLightLocked(const LightState& state) {
    if (mFadeActive || mScreenOn || isLit(mNotificationState)) {
		mRearRed.clear(); mRearRed << 0 << std::endl;
        mRearGreen.clear(); mRearGreen << 0 << std::endl;
        mRearBlue.clear(); mRearBlue << 0 << std::endl;
        mRearEnable.clear(); mRearEnable << 0 << std::endl;
        return;
    }

    uint32_t color = state.color & 0x00ffffff;
    mRearEnable.clear(); 
    
    if (color > 0) {
        mRearEnable << 1 << std::endl; 
        mRearRed.clear(); mRearRed << ((color >> 16) & 0xFF) << std::endl;
        mRearGreen.clear(); mRearGreen << ((color >> 8) & 0xFF) << std::endl;
        mRearBlue.clear(); mRearBlue << (color & 0xFF) << std::endl;
    } else {
        mRearRed << 0 << std::endl;
        mRearGreen << 0 << std::endl;
        mRearBlue << 0 << std::endl;
        mRearEnable << 0 << std::endl;
    }
}

void Light::setNotificationLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mNotificationState = state;
    setSpeakerBatteryLightLocked();
	setRearNotificationLightLocked(state);
}

void Light::setSpeakerBatteryLightLocked() {
    if (isLit(mNotificationState)) {
        setSpeakerLightLocked(mNotificationState);
    } else if (isLit(mAttentionState)) {
        setSpeakerLightLocked(mAttentionState);
    } else if (isLit(mBatteryState)) {
        setSpeakerLightLocked(mBatteryState);
    } else {
        /* Lights off */
        mBlinkPattern << "0x0,-1,-1" << std::endl;
    }
}

void Light::setSpeakerLightLocked(const LightState& state) {
    int onMS, offMS;
    uint32_t color;
    char blink_pattern[PAGE_SIZE];

    switch (state.flashMode) {
        case Flash::TIMED:
            onMS = state.flashOnMs;
            offMS = state.flashOffMs;
            break;
        case Flash::NONE:
        default:
            onMS = -1;
            offMS = -1;
            break;
    }

    color = state.color & 0x00ffffff;

    ALOGD("%s: inColor=0x%08x delay_on=%d, delay_off=%d", __func__, color,
          onMS, offMS);

    sprintf(blink_pattern, "0x%x,%d,%d", color, onMS, offMS);
    mBlinkPattern << blink_pattern << std::endl;
}

void Light::setRearNotificationLightLocked(const LightState& state) {

    uint32_t color = state.color & 0x00ffffff;

    if (color > 0 && !mScreenOn) {
        static bool isNotificationBlinking = false;
        if (isNotificationBlinking) return;

        std::thread([this, color]() {
            while (isLit(mNotificationState) && !mScreenOn) {
                mRearEnable.clear(); mRearEnable << 1 << std::endl;
                mRearRed.clear(); mRearRed << ((color >> 16) & 0xFF) << std::endl;
                mRearGreen.clear(); mRearGreen << ((color >> 8) & 0xFF) << std::endl;
                mRearBlue.clear(); mRearBlue << (color & 0xFF) << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1초 켬

                mRearRed.clear(); mRearRed << 0 << std::endl;
                mRearGreen.clear(); mRearGreen << 0 << std::endl;
                mRearBlue.clear(); mRearBlue << 0 << std::endl;
                mRearEnable.clear(); mRearEnable << 0 << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1초 끔
            }
        }).detach();
    } else {
        mRearRed.clear(); mRearRed << 0 << std::endl;
        mRearGreen.clear(); mRearGreen << 0 << std::endl;
        mRearBlue.clear(); mRearBlue << 0 << std::endl;
        mRearEnable.clear(); mRearEnable << 0 << std::endl;
        setRearBatteryLightLocked(mBatteryState);
    }
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android