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

#include "Box.h"
#include "Application.h"
#include "CanvasRenderer.h"
#include "Debug.h"
#include "memory/Memory.h"
#include "graphics/Camera.h"
#include "graphics/Texture.h"


namespace Viry3D
{
	
	int Mesh2D::GetTextureOrImageWidth() const
	{
		return texture ? texture->GetWidth() : image->width;
	}

	int Mesh2D::GetTextureOrImageHeight() const
	{
		return texture ? texture->GetHeight() : image->height;
	}

	const String Box::AppRoot = Application::Instance()->GetDataPath();

	Box::Box()
	{
		
	}

	Box::~Box()
	{
	
	}

	void Box::OnAddToCanvas(CanvasRenderer* canvas)
	{
		assert(m_canvas == nullptr);
		m_canvas = canvas;
	}

	void Box::OnRemoveFromCanvas(CanvasRenderer* canvas)
	{
		assert(m_canvas == canvas);
		m_canvas = nullptr;
	}

    CanvasRenderer* Box::GetCanvas() const
    {
        if (m_canvas)
        {
            return m_canvas; 
        }
        else if (m_parent)
        {
            return m_parent->GetCanvas();
        }

        return nullptr;
    }

    void Box::AddChild(const Ref<Box>& view)
    {
        assert(view->m_parent == nullptr);
        view->m_parent = this;

		m_childs.Add(view);

        this->MarkCanvasDirty();
    }

    void Box::RemoveChild(const Ref<Box>& view)
    {
        assert(view->m_parent == this);
        view->m_parent = nullptr;
        
		m_childs.Remove(view);

        this->MarkCanvasDirty();
    }

    void Box::RemoveChildAll()
    {
        Vector<Ref<Box>> subviews;
        for (int i = 0; i < this->GetChildCount(); ++i)
        {
            subviews.Add(this->GetChhild(i));
        }
        for (int i = 0; i < subviews.Size(); ++i)
        {
            this->RemoveChild(subviews[i]);
        }
    }


	void Box::SetPivot(const Vector2& pivot)
	{
		m_pivot = pivot;
        this->MarkCanvasDirty();
	}
	void Box::SetColor(const String& color)
	{
		m_color = Color::Parse(color);
		this->MarkCanvasDirty();
	}
	void Box::SetColor(const Color& color)
	{
		m_color = color;
		this->MarkCanvasDirty();
	}

	void Box::SetSize(int w, int h)
	{
		if (m_size.x != w || m_size.y != h) {
			m_size.x = w;
			m_size.y = h;
			this->MarkCanvasDirty();
		}
	}
	void Box::SetSize(const Vector2i& size)
	{
		if (m_size != size) {
			m_size = size;
			this->MarkCanvasDirty();
		}
	}

    Vector2i Box::GetCalculateddSize()
    {
        Vector2i size = m_size;
        
        if (size.x == 0 || size.y == 0)
        {
            Vector2i parent_size;

            if (m_parent)
            {
                parent_size = m_parent->GetCalculateddSize();
            }
            else
            {
                parent_size.x = m_canvas->GetCamera()->GetTargetWidth();
                parent_size.y = m_canvas->GetCamera()->GetTargetHeight();
            }

            if (size.x == 0)
            {
                size.x = parent_size.x;
            }

            if (size.y == 0)
            {
                size.y = parent_size.y;
            }
        }

        return size;
    }
	void Box::SetPosition(int x,int y)
	{
		if (m_position.x != x || m_position.y != y) {
			m_position.x = x;
			m_position.y = y;
			this->MarkCanvasDirty();
		}
	}
	void Box::SetPosition(const Vector2i& position)
	{
		if (m_position != position) {
			m_position = position;
			this->MarkCanvasDirty();
		}
	}

    void Box::SetRotation(float rotation)
    {
		m_local_rotation = rotation;
        this->MarkCanvasDirty();
    }
	void Box::SetScale(float x, float y)
	{
		m_local_scale.x = x;
		m_local_scale.y = y;
		this->MarkCanvasDirty();
	}
    void Box::SetScale(const Vector2& scale)
    {
        m_local_scale = scale;
        this->MarkCanvasDirty();
    }
	void Box::SetLeft(int v) {
		left = (float)v;
		this->MarkCanvasDirty();
	}
	void Box::SetTop(int v) {
		top = (float)v;
		this->MarkCanvasDirty();
	}
	void Box::SetRight(int v) {
		right = (float)v;
		this->MarkCanvasDirty();
	}
	void Box::SetBottom(int v) {
		bottom = (float)v;
		this->MarkCanvasDirty();
	}
	void Box::SetCenterX(int v) {
		centerX = (float)v;
		this->MarkCanvasDirty();
	}
	void Box::SetCenterY(int v) {
		centerY = (float)v;
		this->MarkCanvasDirty();
	}
    void Box::UpdateLayout()
    {
        Rect parent_rect;

        if (m_parent)
        {
            parent_rect = m_parent->GetRect();
        }
        else
        {
            parent_rect = Rect(0, 0, (float) m_canvas->GetCamera()->GetTargetWidth(), (float) m_canvas->GetCamera()->GetTargetHeight());
        }


        Vector2i size = this->GetSize();
        if (size.x == 0)
        {
            size.x = (int) parent_rect.width;
        }
        if (size.y == 0)
        {
            size.y = (int) parent_rect.height;
        }
		Vector2i local_pos;
		if (left != DEF) {
			local_pos.x = left;
		}
		if (top != DEF) {
			local_pos.y = top;
		}
		if (right != DEF) {
			local_pos.x = parent_rect.width - size.x - right;
		}
		if (bottom != DEF) {
			local_pos.y = parent_rect.height - size.y - bottom;
		}
		if (left != DEF && right != DEF) {
			local_pos.x = left;
			size.x = parent_rect.width - right;
		}
		if (top != DEF && bottom != DEF) {
			local_pos.y = top;
			size.y = parent_rect.height - bottom;
		}
		if (centerX != DEF) {
			local_pos.x = (parent_rect.width - size.x)*0.5;
		}
		if (centerY != DEF) {
			local_pos.y = (parent_rect.height - size.y)*0.5;
		}

		local_pos = m_position;


        m_rect.x = parent_rect.x + local_pos.x - Mathf::Round(m_pivot.x * size.x);
        m_rect.y = parent_rect.y + local_pos.y - Mathf::Round(m_pivot.y * size.y);
        m_rect.width = (float) size.x;
        m_rect.height = (float) size.y;

        if (m_parent)
        {
            m_rotation = m_parent->GetRotation() + m_local_rotation;

            Vector2 parent_scale = m_parent->GetScale();
            m_scale = Vector2(parent_scale.x * m_local_scale.x, parent_scale.y * m_local_scale.y);
        }
        else
        {
            m_rotation = m_local_rotation;
            m_scale = m_local_scale;
        }

        for (auto& i : m_childs)
        {
            i->UpdateLayout();
        }
    }

    void Box::OnResize(int width, int height)
    {
        for (auto& i : m_childs)
        {
            i->UpdateLayout();
        }
    }

	void Box::MarkCanvasDirty() const
	{
		CanvasRenderer* canvas = this->GetCanvas();
		if (canvas)
		{
			canvas->MarkCanvasDirty();
		}
	}

    void Box::ComputeVerticesRectAndMatrix(Rect& rect, Matrix4x4& matrix)
    {
        int x = (int) m_rect.x;
        int y = (int) -m_rect.y;

        rect = Rect((float) x, (float) y, m_rect.width, m_rect.height);

        Vector3 pivot_pos;
        pivot_pos.x = x + Mathf::Round(m_pivot.x * m_rect.width);
        pivot_pos.y = y - Mathf::Round(m_pivot.y * m_rect.height);
        pivot_pos.z = 0;
        matrix = Matrix4x4::Translation(pivot_pos) * Matrix4x4::Rotation(Quaternion::AngleAxis(-m_rotation, Vector3(0,0,1))) * Matrix4x4::Scaling(m_scale) * Matrix4x4::Translation(-pivot_pos);
    }

	void Box::FillMeshes(Vector<Mesh2D>& meshes)
	{
		for (auto& i : m_childs)
		{
			i->FillMeshes(meshes);
		}
	}

    bool Box::OnTouchDownInside(const Vector2i& pos) const
    {
        if (m_on_touch_down_inside)
        {
            return m_on_touch_down_inside(pos);
        }
        return false;
    }

    bool Box::OnTouchMoveInside(const Vector2i& pos) const
    {
        if (m_on_touch_move_inside)
        {
            return m_on_touch_move_inside(pos);
        }
        return false;
    }
    
    bool Box::OnTouchUpInside(const Vector2i& pos) const
    {
        if (m_on_touch_up_inside)
        {
            return m_on_touch_up_inside(pos);
        }
        return false;
    }

    bool Box::OnTouchUpOutside(const Vector2i& pos) const
    {
        if (m_on_touch_up_outside)
        {
            return m_on_touch_up_outside(pos);
        }
        return false;
    }

    bool Box::OnTouchDrag(const Vector2i& pos) const
    {
        if (m_on_touch_drag)
        {
            return m_on_touch_drag(pos);
        }
        return false;
    }
}
