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

#pragma once

#include "Sprite.h"

namespace Viry3D
{
    class Label;

    class InputField : public Sprite
    {
    public:
        InputField();
        virtual ~InputField();
        void SetPlaceholderText(const String& placeholder);
        void SetPlaceholderTextColor(const Color& color);
        void SetCaretBlinkRate(float rate);
        const String& GetText() const;
        void SetText(const String& text);

    private:
        void OnGotFocus();
        void OnLostFocus();
        void SetCaretPos(int line, int index);

    private:
        Ref<Label> m_placeholder;
        Ref<Label> m_label;
        Ref<Sprite> m_caret;
        float m_caret_blink_rate;
        bool m_touch_down;
        bool m_focused;
        Vector4 m_label_margin;
    };
}
