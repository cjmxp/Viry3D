/*
* Viry3D
* Copyright 2014-2018 by Stack - stackos@qq.com
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
#include "Texture.h"
#include "Renderer.h"
#include "Material.h"
#include "Shader.h"
#include "Debug.h"

namespace Viry3D
{
	Camera::Camera():
#if VR_VULKAN
        m_render_pass(VK_NULL_HANDLE),
        m_cmd_pool(VK_NULL_HANDLE),
        m_render_pass_dirty(true),
        m_instance_cmds_dirty(true),
#elif VR_GLES
        m_framebuffer(0),
#endif
		m_renderer_order_dirty(true),
		m_clear_flags(CameraClearFlags::ColorAndDepth),
		m_clear_color(0, 0, 0, 1),
		m_viewport_rect(0, 0, 1, 1),
		m_depth(0),
        m_view_matrix_dirty(true),
        m_projection_matrix_dirty(true),
        m_field_of_view(45),
        m_near_clip(0.3f),
        m_far_clip(1000),
        m_orthographic(false),
        m_orthographic_size(1),
        m_view_matrix_external(false),
        m_projection_matrix_external(false)
	{

	}

	Camera::~Camera()
	{
#if VR_VULKAN
		this->ClearRenderPass();
		this->ClearInstanceCmds();
#elif VR_GLES
        if (m_framebuffer)
        {
            glDeleteFramebuffers(1, &m_framebuffer);
        }
#endif
	}

    void Camera::OnMatrixDirty()
    {
        m_view_matrix_dirty = true;
    }

    void Camera::SetViewMatrixExternal(const Matrix4x4& mat)
    {
        m_view_matrix = mat;
        m_view_matrix_dirty = true;
        m_view_matrix_external = true;
    }
    
    const Matrix4x4& Camera::GetViewMatrix()
    {
        if (m_view_matrix_dirty)
        {
            m_view_matrix_dirty = false;

            if (!m_view_matrix_external)
            {
                m_view_matrix = Matrix4x4::LookTo(this->GetPosition(), this->GetForward(), this->GetUp());
            }

            for (auto& i : m_renderers)
            {
                const Ref<Material>& material = i.renderer->GetMaterial();
                if (material)
                {
                    material->SetMatrix(VIEW_MATRIX, m_view_matrix);
                    material->SetVector(CAMERA_POSITION, this->GetPosition());
                }
            }
        }

        return m_view_matrix;
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

    void Camera::SetProjectionMatrixExternal(const Matrix4x4& mat)
    {
        m_projection_matrix = mat;
        m_projection_matrix_dirty = true;
        m_projection_matrix_external = true;
    }
    
    const Matrix4x4& Camera::GetProjectionMatrix()
    {
        if (m_projection_matrix_dirty)
        {
            m_projection_matrix_dirty = false;

            if (!m_projection_matrix_external)
            {
                float view_width = this->GetTargetWidth() * m_viewport_rect.width;
                float view_height = this->GetTargetHeight() * m_viewport_rect.height;
                
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

            for (auto& i : m_renderers)
            {
                const Ref<Material>& material = i.renderer->GetMaterial();
                if (material)
                {
                    material->SetMatrix(PROJECTION_MATRIX, m_projection_matrix);
                }
            }
        }

        return m_projection_matrix;
    }

	void Camera::SetClearFlags(CameraClearFlags flags)
	{
		m_clear_flags = flags;

#if VR_VULKAN
		m_render_pass_dirty = true;
#endif
	}

	void Camera::SetClearColor(const Color& color)
	{
		m_clear_color = color;

#if VR_VULKAN
		Display::Instance()->MarkPrimaryCmdDirty();
#endif
	}

	void Camera::SetViewportRect(const Rect& rect)
	{
		m_viewport_rect = rect;
        m_projection_matrix_dirty = true;
        
#if VR_VULKAN
		m_instance_cmds_dirty = true;
#endif
	}

	void Camera::SetDepth(int depth)
	{
		m_depth = depth;

#if VR_VULKAN
		Display::Instance()->MarkPrimaryCmdDirty();
#endif
	}

	void Camera::SetRenderTarget(const Ref<Texture>& color_texture, const Ref<Texture>& depth_texture)
	{
		m_render_target_color = color_texture;
		m_render_target_depth = depth_texture;

#if VR_VULKAN
		m_render_pass_dirty = true;
#endif
	}

	void Camera::Update()
	{
        if (m_view_matrix_dirty)
        {
            this->GetViewMatrix();
        }

        if (m_projection_matrix_dirty)
        {
            this->GetProjectionMatrix();
        }

#if VR_VULKAN
		if (m_render_pass_dirty)
		{
			m_render_pass_dirty = false;
			this->UpdateRenderPass();

			m_instance_cmds_dirty = true;
			Display::Instance()->MarkPrimaryCmdDirty();
		}
#endif

		if (m_renderer_order_dirty)
		{
			m_renderer_order_dirty = false;
			this->SortRenderers();

#if VR_VULKAN
			Display::Instance()->MarkPrimaryCmdDirty();
#endif
		}

		this->UpdateRenderers();

#if VR_VULKAN
		this->UpdateInstanceCmds();
#endif
	}

#if VR_GLES
    void Camera::OnDraw()
    {
        this->BindTarget();
        this->ClearTarget();

        for (auto& i : m_renderers)
        {
            i.renderer->OnDraw();
        }
    }

    void Camera::BindTarget()
    {
        if (this->HasRenderTarget())
        {
            if (m_framebuffer == 0)
            {
                glGenFramebuffers(1, &m_framebuffer);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            
            if (m_render_target_color)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_render_target_color->GetTexture(), 0);
            }

            if (m_render_target_depth)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_render_target_depth->GetTexture(), 0);
            }

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                Log("frame buffer attachment error: %x", status);
            }
        }
        else
        {
#if VR_IOS
            Display::Instance()->BindDefaultFramebuffer();
#else
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
        }

        int x = (int) (this->GetTargetWidth() * m_viewport_rect.x);
        int y = (int) (this->GetTargetHeight() * m_viewport_rect.y);
        int w = (int) (this->GetTargetWidth() * m_viewport_rect.width);
        int h = (int) (this->GetTargetHeight() * m_viewport_rect.height);
        y = this->GetTargetHeight() - (y + h);
        glViewport(x, y, w, h);
    }

    void Camera::ClearTarget()
    {
        GLbitfield clear_bits = 0;

        switch (m_clear_flags)
        {
        case CameraClearFlags::Nothing:
        case CameraClearFlags::Invalidate:
            break;
        case CameraClearFlags::Color:
            clear_bits = GL_COLOR_BUFFER_BIT;
            break;
        case CameraClearFlags::Depth:
            clear_bits = GL_DEPTH_BUFFER_BIT;
            break;
        case CameraClearFlags::ColorAndDepth:
            clear_bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            break;
        }

        if (clear_bits)
        {
            if (clear_bits & GL_COLOR_BUFFER_BIT)
            {
                glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }

            if (clear_bits & GL_DEPTH_BUFFER_BIT)
            {
                glDepthMask(GL_TRUE);
            }
            
            glClear(clear_bits);
        }
    }
#endif

    void Camera::OnFrameEnd()
    {
        for (auto& i : m_renderers)
        {
            i.renderer->OnFrameEnd();
        }
    }

	void Camera::OnResize(int width, int height)
	{
#if VR_VULKAN
		this->ClearRenderPass();
		this->ClearInstanceCmds();

        m_render_pass_dirty = true;
        m_instance_cmds_dirty = true;
#endif
		
        m_projection_matrix_dirty = true;

        for (auto& i : m_renderers)
        {
            i.renderer->OnResize(width, height);
        }
	}

	void Camera::OnPause()
	{
#if VR_VULKAN
        this->ClearRenderPass();
        this->ClearInstanceCmds();

        m_render_pass_dirty = true;
        m_instance_cmds_dirty = true;
#endif
	}

	void Camera::SortRenderers()
	{
		m_renderers.Sort([](const RendererInstance& a, const RendererInstance& b) {
			const Ref<Material>& ma = a.renderer->GetMaterial();
			const Ref<Material>& mb = b.renderer->GetMaterial();
			if (ma && mb)
			{
				return ma->GetQueue() < mb->GetQueue();
			}
			else if (!ma && mb)
			{
				return true;
			}
			else
			{
				return false;
			}
		});
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
            return Display::Instance()->GetWidth();
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
            return Display::Instance()->GetHeight();
        }
    }

    void Camera::AddRenderer(const Ref<Renderer>& renderer)
    {
        RendererInstance instance;
        instance.renderer = renderer;

        if (!m_renderers.Contains(instance))
        {
            const Ref<Material>& material = renderer->GetMaterial();
            if (material)
            {
                material->SetMatrix(VIEW_MATRIX, this->GetViewMatrix());
                material->SetMatrix(PROJECTION_MATRIX, this->GetProjectionMatrix());
                material->SetVector(CAMERA_POSITION, this->GetPosition());
            }

            m_renderers.AddLast(instance);
            this->MarkRendererOrderDirty();

            renderer->OnAddToCamera(this);
        }
    }

    void Camera::RemoveRenderer(const Ref<Renderer>& renderer)
    {
#if VR_VULKAN
        VkDevice device = Display::Instance()->GetDevice();
        Display::Instance()->WaitDevice();
#endif

        for (auto i = m_renderers.begin(); i != m_renderers.end(); ++i)
        {
            if (i->renderer == renderer)
            {
#if VR_VULKAN
                if (i->cmd)
                {
                    vkFreeCommandBuffers(device, m_cmd_pool, 1, &i->cmd);
                }
#endif
                m_renderers.Remove(i);
                break;
            }
        }

#if VR_VULKAN
        Display::Instance()->MarkPrimaryCmdDirty();
#endif

        renderer->OnRemoveFromCamera(this);
    }

    void Camera::MarkRendererOrderDirty()
    {
        m_renderer_order_dirty = true;
    }

    void Camera::UpdateRenderers()
    {
        for (auto& i : m_renderers)
        {
            i.renderer->Update();
        }
    }

#if VR_VULKAN
    void Camera::UpdateRenderPass()
    {
        this->ClearRenderPass();

        Display::Instance()->CreateRenderPass(
            m_render_target_color,
            m_render_target_depth,
            m_clear_flags,
            &m_render_pass,
            m_framebuffers);
    }

    void Camera::ClearRenderPass()
    {
        VkDevice device = Display::Instance()->GetDevice();

        for (int i = 0; i < m_framebuffers.Size(); ++i)
        {
            vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
        }
        m_framebuffers.Clear();

        if (m_render_pass)
        {
            Shader::OnRenderPassDestroy(m_render_pass);
            vkDestroyRenderPass(device, m_render_pass, nullptr);
            m_render_pass = VK_NULL_HANDLE;
        }
    }

    void Camera::UpdateInstanceCmds()
    {
        for (auto& i : m_renderers)
        {
            if (i.cmd_dirty || m_instance_cmds_dirty)
            {
                i.cmd_dirty = false;

                if (i.cmd == VK_NULL_HANDLE)
                {
                    if (m_cmd_pool == VK_NULL_HANDLE)
                    {
                        Display::Instance()->CreateCommandPool(&m_cmd_pool);
                    }

                    Display::Instance()->CreateCommandBuffer(m_cmd_pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, &i.cmd);
                }

                this->BuildInstanceCmd(i.cmd, i.renderer);

                Display::Instance()->MarkPrimaryCmdDirty();
            }
        }

        m_instance_cmds_dirty = false;
    }

    void Camera::ClearInstanceCmds()
    {
        VkDevice device = Display::Instance()->GetDevice();

        for (auto& i : m_renderers)
        {
            if (i.cmd)
            {
                vkFreeCommandBuffers(device, m_cmd_pool, 1, &i.cmd);
                i.cmd = VK_NULL_HANDLE;
            }
        }

        if (m_cmd_pool)
        {
            vkDestroyCommandPool(device, m_cmd_pool, nullptr);
            m_cmd_pool = VK_NULL_HANDLE;
        }
    }

    Vector<VkCommandBuffer> Camera::GetInstanceCmds() const
    {
        Vector<VkCommandBuffer> cmds;

        for (const auto& i : m_renderers)
        {
            cmds.Add(i.cmd);
        }

        return cmds;
    }

    VkFramebuffer Camera::GetFramebuffer(int index) const
    {
        if (this->HasRenderTarget())
        {
            return m_framebuffers[0];
        }
        else
        {
            return m_framebuffers[index];
        }
    }

    void Camera::MarkInstanceCmdDirty(Renderer* renderer)
    {
        for (auto& i : m_renderers)
        {
            if (i.renderer.get() == renderer)
            {
                i.cmd_dirty = true;
                break;
            }
        }
    }

    void Camera::BuildInstanceCmd(VkCommandBuffer cmd, const Ref<Renderer>& renderer)
    {
        const Ref<Material>& material = renderer->GetMaterial();
        Ref<BufferObject> vertex_buffer = renderer->GetVertexBuffer();
        Ref<BufferObject> index_buffer = renderer->GetIndexBuffer();
        Ref<BufferObject> draw_buffer = renderer->GetDrawBuffer();

        if (!material || !vertex_buffer || !index_buffer || !draw_buffer)
        {
            Display::Instance()->BuildEmptyInstanceCmd(cmd, m_render_pass);
            return;
        }

        const Ref<Material>& instance_material = renderer->GetInstanceMaterial();
        const Ref<Shader>& shader = material->GetShader();

        Vector<VkDescriptorSet> descriptor_sets = material->GetDescriptorSets();

        if (instance_material)
        {
            const Vector<VkDescriptorSet>& instance_descriptor_sets = instance_material->GetDescriptorSets();
            const Map<String, MaterialProperty>& instance_properties = instance_material->GetProperties();
            for (const auto& i : instance_properties)
            {
                int instance_set_index = instance_material->FindUniformSetIndex(i.second.name);
                if (instance_set_index >= 0)
                {
                    descriptor_sets[instance_set_index] = instance_descriptor_sets[instance_set_index];
                }
            }
        }

        bool color_attachment = true;
        bool depth_attachment = true;
        int sample_count = 1;
        if (this->HasRenderTarget())
        {
            color_attachment = (bool) this->GetRenderTargetColor();
            depth_attachment = (bool) this->GetRenderTargetDepth();
            if (color_attachment)
            {
                sample_count = this->GetRenderTargetColor()->GetSampleCount();
            }
            if (depth_attachment)
            {
                sample_count = this->GetRenderTargetDepth()->GetSampleCount();
            }
        }

        Display::Instance()->BuildInstanceCmd(
            cmd,
            m_render_pass,
            shader->GetPipelineLayout(),
            shader->GetPipeline(m_render_pass, color_attachment, depth_attachment, sample_count),
            descriptor_sets,
            this->GetTargetWidth(),
            this->GetTargetHeight(),
            m_viewport_rect,
            vertex_buffer,
            index_buffer,
            draw_buffer);
    }
#endif
}
