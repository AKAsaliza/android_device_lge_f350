/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

#define BTM_DEF_LOCAL_NAME   "LG G PRO 2"
#define BTA_DISABLE_DELAY 1000 /* in milliseconds */

/* Allow car handsfree setup to work (BMW and Mercedes) */
#define BTM_WBS_INCLUDED TRUE        /* Enable WBS */
#define BTIF_HF_WBS_PREFERRED FALSE  /* Don't default to WBS */

#endif
