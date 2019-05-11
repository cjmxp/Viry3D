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

#include "Camera.h"
#include "GameObject.h"
#include "Texture.h"
#include "Engine.h"
#include "Renderer.h"

namespace Viry3D
{
	List<Camera*> Camera::m_cameras;
	bool Camera::m_cameras_order_dirty = false;

	void Camera::RenderAll()
	{
		if (m_cameras_order_dirty)
		{
			m_cameras_order_dirty = false;
			m_cameras.Sort([](Camera* a, Camera* b) {
				return a->GetDepth() < b->GetDepth();
			});
		}

		for (auto i : m_cameras)
		{
			i->Render();
		}
	}

	void Camera::Render()
	{
		auto& driver = Engine::Instance()->GetDriverApi();

		int target_width = this->GetTargetWidth();
		int target_height = this->GetTargetHeight();

		filament::backend::RenderTargetHandle target;
		filament::backend::RenderPassParams params;
		params.flags.clear = filament::backend::TargetBufferFlags::NONE;
		params.flags.discardStart = filament::backend::TargetBufferFlags::NONE;
		params.flags.discardEnd = filament::backend::TargetBufferFlags::NONE;

		if (m_render_target_color || m_render_target_depth)
		{
			// TODO:
			// set target

			switch (m_clear_flags)
			{
			case CameraClearFlags::Invalidate:
				params.flags.clear = filament::backend::TargetBufferFlags::NONE;
				if (m_render_target_color)
				{
					params.flags.discardStart |= filament::backend::TargetBufferFlags::COLOR;
				}
				if (m_render_target_depth)
				{
					params.flags.discardStart |= filament::backend::TargetBufferFlags::DEPTH;
				}
				break;
			case CameraClearFlags::Color:
				if (m_render_target_color)
				{
					params.flags.clear = filament::backend::TargetBufferFlags::COLOR;
				}
				break;
			case CameraClearFlags::Depth:
				if (m_render_target_depth)
				{
					params.flags.clear = filament::backend::TargetBufferFlags::DEPTH;
				}
				break;
			case CameraClearFlags::ColorAndDepth:
				if (m_render_target_color)
				{
					params.flags.clear = filament::backend::TargetBufferFlags::COLOR;
				}
				if (m_render_target_depth)
				{
					params.flags.clear = filament::backend::TargetBufferFlags::DEPTH;
				}
				break;
			case CameraClearFlags::Nothing:
				params.flags.clear = filament::backend::TargetBufferFlags::NONE;
				break;
			}
		}
		else
		{
			target = *(filament::backend::RenderTargetHandle*) Engine::Instance()->GetDefaultRenderTarget();

			switch (m_clear_flags)
			{
			case CameraClearFlags::Invalidate:
				params.flags.clear = filament::backend::TargetBufferFlags::NONE;
				params.flags.discardStart = filament::backend::TargetBufferFlags::COLOR_AND_DEPTH;
				break;
			case CameraClearFlags::Color:
				params.flags.clear = filament::backend::TargetBufferFlags::COLOR;
				break;
			case CameraClearFlags::Depth:
				params.flags.clear = filament::backend::TargetBufferFlags::DEPTH;
				break;
			case CameraClearFlags::ColorAndDepth:
				params.flags.clear = filament::backend::TargetBufferFlags::COLOR_AND_DEPTH;
				break;
			case CameraClearFlags::Nothing:
				params.flags.clear = filament::backend::TargetBufferFlags::NONE;
				break;
			}
		}

		params.viewport.left = (int32_t) (m_viewport_rect.x * target_width);
		params.viewport.bottom = (int32_t) ((1.0f - (m_viewport_rect.y + m_viewport_rect.h)) * target_height);
		params.viewport.width = (uint32_t) (m_viewport_rect.w * target_width);
		params.viewport.height = (uint32_t) (m_viewport_rect.h * target_height);
		params.clearColor = filament::math::float4(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);

		driver.beginRenderPass(target, params);

        this->RenderPass();
        
		driver.endRenderPass();

		driver.flush();
	}
    
    void Camera::RenderPass()
    {
        List<Renderer*> renderers;
        this->CullRenderers(Renderer::GetRenderers(), renderers);
     
        for (auto i : renderers)
        {
            
        }
    }
    
    void Camera::CullRenderers(const List<Renderer*>& renderers, List<Renderer*>& result)
    {
        for (auto i : renderers)
        {
            int layer = i->GetGameObject()->GetLayer();
            if ((1 << layer) & m_culling_mask)
            {
                result.AddLast(i);
            }
        }
        result.Sort([](Renderer* a, Renderer* b) {
            return false;
        });
    }

	Camera::Camera():
		m_depth(0),
        m_culling_mask(0xffffffff),
		m_clear_flags(CameraClearFlags::ColorAndDepth),
		m_clear_color(0, 0, 0, 1),
		m_viewport_rect(0, 0, 1, 1),
		m_field_of_view(45),
		m_near_clip(0.3f),
		m_far_clip(1000),
		m_orthographic(false),
		m_orthographic_size(1),
		m_view_matrix_dirty(true),
		m_projection_matrix_dirty(true),
		m_view_matrix_external(false),
		m_projection_matrix_external(false)
    {
		m_cameras.AddLast(this);
		m_cameras_order_dirty = true;
    }
    
	Camera::~Camera()
    {
        
    }

	void Camera::SetDepth(int depth)
	{
		m_depth = depth;
	}

    void Camera::SetCullingMask(uint32_t mask)
    {
        m_culling_mask = mask;
    }
    
	void Camera::SetClearFlags(CameraClearFlags flags)
	{
		m_clear_flags = flags;
	}

	void Camera::SetClearColor(const Color& color)
	{
		m_clear_color = color;
	}

	void Camera::SetViewportRect(const Rect& rect)
	{
		m_viewport_rect = rect;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetFieldOfView(float fov)
	{
		m_field_of_view = fov;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetNearClip(float clip)
	{
		m_near_clip = clip;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetFarClip(float clip)
	{
		m_far_clip = clip;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetOrthographic(bool enable)
	{
		m_orthographic = enable;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetOrthographicSize(float size)
	{
		m_orthographic_size = size;
		m_projection_matrix_dirty = true;
	}

	const Matrix4x4& Camera::GetViewMatrix()
	{
		if (m_view_matrix_dirty)
		{
			m_view_matrix_dirty = false;

			if (!m_view_matrix_external)
			{
				m_view_matrix = Matrix4x4::LookTo(this->GetTransform()->GetPosition(), this->GetTransform()->GetForward(), this->GetTransform()->GetUp());
			}
		}

		return m_view_matrix;
	}

	const Matrix4x4& Camera::GetProjectionMatrix()
	{
		if (m_projection_matrix_dirty)
		{
			m_projection_matrix_dirty = false;

			if (!m_projection_matrix_external)
			{
				float view_width = this->GetTargetWidth() * m_viewport_rect.w;
				float view_height = this->GetTargetHeight() * m_viewport_rect.h;

				if (m_orthographic)
				{
					float ortho_size = m_orthographic_size;
					float top = ortho_size;
					float bottom = -ortho_size;
					float plane_h = ortho_size * 2;
					float plane_w = plane_h * view_width / view_height;
					m_projection_matrix = Matrix4x4::Ortho(-plane_w / 2, plane_w / 2, bottom, top, m_near_clip, m_far_clip);
				}
				else
				{
					m_projection_matrix = Matrix4x4::Perspective(m_field_of_view, view_width / view_height, m_near_clip, m_far_clip);
				}
			}
		}

		return m_projection_matrix;
	}

	void Camera::SetViewMatrixExternal(const Matrix4x4& mat)
	{
		m_view_matrix = mat;
		m_view_matrix_dirty = true;
		m_view_matrix_external = true;
	}

	void Camera::SetProjectionMatrixExternal(const Matrix4x4& mat)
	{
		m_projection_matrix = mat;
		m_projection_matrix_dirty = true;
		m_projection_matrix_external = true;
	}

	void Camera::SetRenderTargetColor(const Ref<Texture>& texture)
	{
		m_render_target_color = texture;
		m_projection_matrix_dirty = true;
	}

	void Camera::SetRenderTargetDepth(const Ref<Texture>& texture)
	{
		m_render_target_depth = texture;
		m_projection_matrix_dirty = true;
	}

	int Camera::GetTargetWidth() const
	{
		if (m_render_target_color)
		{
			return m_render_target_color->GetWidth();
		}
		else if (m_render_target_depth)
		{
			return m_render_target_depth->GetWidth();
		}
		else
		{
			return Engine::Instance()->GetWidth();
		}
	}

	int Camera::GetTargetHeight() const
	{
		if (m_render_target_color)
		{
			return m_render_target_color->GetHeight();
		}
		else if (m_render_target_depth)
		{
			return m_render_target_depth->GetHeight();
		}
		else
		{
			return Engine::Instance()->GetHeight();
		}
	}
}
