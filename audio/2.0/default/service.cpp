/*
 * Copyright (C) 2016 The Android Open Source Project
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

#define LOG_TAG "audiohalservice"

#include <android/hardware/audio/2.0/IDevicesFactory.h>
#include <android/hardware/audio/effect/2.0/IEffectsFactory.h>
#include <android/hardware/soundtrigger/2.0/ISoundTriggerHw.h>
#include <android/hardware/soundtrigger/2.1/ISoundTriggerHw.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/LegacySupport.h>

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::registerPassthroughServiceImplementation;

using android::hardware::audio::effect::V2_0::IEffectsFactory;
using android::hardware::audio::V2_0::IDevicesFactory;
using V2_0_ISoundTriggerHw = android::hardware::soundtrigger::V2_0::ISoundTriggerHw;
using V2_1_ISoundTriggerHw = android::hardware::soundtrigger::V2_1::ISoundTriggerHw;
using android::hardware::registerPassthroughServiceImplementation;

using android::OK;

int main(int /* argc */, char* /* argv */ []) {
    android::ProcessState::initWithDriver("/dev/vndbinder");
    // start a threadpool for vndbinder interactions
    android::ProcessState::self()->startThreadPool();
    configureRpcThreadpool(16, true /*callerWillJoin*/);
    android::status_t status;
    status = registerPassthroughServiceImplementation<IDevicesFactory>();
    LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering audio service: %d", status);
    status = registerPassthroughServiceImplementation<IEffectsFactory>();
    LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering audio effects service: %d", status);
    // Soundtrigger might be not present.
    status = registerPassthroughServiceImplementation<V2_1_ISoundTriggerHw>();
    ALOGW_IF(status != OK, "Registering soundtrigger V2.1 service was unsuccessful: %d", status);
    status = registerPassthroughServiceImplementation<V2_0_ISoundTriggerHw>();
    ALOGW_IF(status != OK, "Registering soundtrigger V2.0 service was unsuccessful: %d", status);
    joinRpcThreadpool();
    return status;
}
