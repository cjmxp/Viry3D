/*
* Viry3D
* Copyright 2014-2019 by Stack - stackos@qq.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.viry3d.app;

import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import com.viry3d.lib.ActivityBase;
import com.viry3d.lib.VRSurfaceView;

public class ActivityMain extends ActivityBase {
    RelativeLayout mLayout;
    VRSurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mLayout = new RelativeLayout(this);

        mSurfaceView = new VRSurfaceView(this);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        mLayout.addView(mSurfaceView, params);

        this.setContentView(mLayout);
    }
}