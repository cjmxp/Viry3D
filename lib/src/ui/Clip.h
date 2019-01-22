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

#include "Box.h"

namespace Viry3D
{
    class Texture;

    class Clip : public Box
    {
    public:
		Clip();
        virtual ~Clip();
		const String& GetSkin() {return m_skin;};
		void SetSkin(const String& skin);
      
    protected:
       // virtual void FillSelfMeshes(Vector<ViewMesh>& meshes);

    private:
        Ref<Texture> m_texture;
		String m_skin;
    };
}
