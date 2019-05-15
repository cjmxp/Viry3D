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

#include "Renderer.h"
#include "Engine.h"
#include "GameObject.h"

namespace Viry3D
{
    List<Renderer*> Renderer::m_renderers;
    
    Renderer::Renderer():
        m_lightmap_scale_offset(1, 1, 0, 0),
        m_lightmap_index(-1)
    {
        m_renderers.AddLast(this);
    }
    
    Renderer::~Renderer()
    {
		auto& driver = Engine::Instance()->GetDriverApi();

		if (m_transform_uniform_buffer)
		{
			driver.destroyUniformBuffer(m_transform_uniform_buffer);
		}

        m_renderers.Remove(this);
    }
    
    Ref<Material> Renderer::GetMaterial() const
    {
        Ref<Material> material;
        
        if (m_materials.Size() > 0)
        {
            material = m_materials[0];
        }
        
        return material;
    }
    
    void Renderer::SetMaterial(const Ref<Material>& material)
    {
        this->SetMaterials({ material });
    }
    
    void Renderer::SetMaterials(const Vector<Ref<Material>>& materials)
    {
        m_materials = materials;
    }
    
    void Renderer::SetLightmapIndex(int index)
    {
        m_lightmap_index = index;
    }
    
    void Renderer::SetLightmapScaleOffset(const Vector4& vec)
    {
        m_lightmap_scale_offset = vec;
    }
    
    filament::backend::RenderPrimitiveHandle Renderer::GetPrimitive(int submesh) const
    {
        filament::backend::RenderPrimitiveHandle primitive;
        return primitive;
    }

	void Renderer::PrepareRender()
	{
		auto& driver = Engine::Instance()->GetDriverApi();
		if (!m_transform_uniform_buffer)
		{
			m_transform_uniform_buffer = driver.createUniformBuffer(sizeof(Matrix4x4), filament::backend::BufferUsage::DYNAMIC);
		}

		void* buffer = Memory::Alloc<void>(sizeof(Matrix4x4));
		Memory::Copy(buffer, &this->GetTransform()->GetLocalToWorldMatrix(), sizeof(Matrix4x4));
		driver.loadUniformBuffer(m_transform_uniform_buffer, filament::backend::BufferDescriptor(buffer, sizeof(Matrix4x4), FreeBufferCallback));
	}
}
