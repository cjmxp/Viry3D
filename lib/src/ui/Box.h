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

#include "graphics/Color.h"
#include "graphics/VertexAttribute.h"
#include "math/Vector2.h"
#include "math/Vector2i.h"
#include "math/Quaternion.h"
#include "math/Rect.h"
#include "math/Matrix4x4.h"
#include <functional>

#define DEF 0.1f

namespace Viry3D
{
	class CanvasRenderer;
	struct ViewAlignment;
	class Texture;
	class Image;
	class Box;

	struct Mesh2D
	{
		Vector<Vertex> vertices;
		Vector<unsigned short> indices;
		Ref<Texture> texture;
		Ref<Image> image;
		Box* view = nullptr;
		bool base_view = false;

		bool HasTextureOrImage() const
		{
			return texture || image;
		}

		int GetTextureOrImageWidth() const;
		int GetTextureOrImageHeight() const;
	};

	class Box
	{
	public:
        typedef std::function<bool(const Vector2i& pos)> InputAction;

		Box();
		virtual ~Box();
        virtual void UpdateLayout();
        virtual void OnResize(int width, int height);
		Vector2i GetCalculateddSize();
		//画布渲染管理
		CanvasRenderer* GetCanvas() const;
		void OnAddToCanvas(CanvasRenderer* canvas);
		void OnRemoveFromCanvas(CanvasRenderer* canvas);
		//子容器管理
		void AddChild(const Ref<Box>& view);
		void RemoveChild(const Ref<Box>& view);
		void RemoveChildAll();
		const Ref<Box>& GetChhild(int index) const { return m_childs[index]; }
		int GetChildCount() const { return m_childs.Size(); }
		//父容器
		Box* GetParent() const { return m_parent; }
		//颜色 默认白色
		const Color& GetColor() const { return m_color; }
		void SetColor(const Color& color);
		void SetColor(const String& color);
		//中心点 取值范围从0.0f到1.0f left top is (0.0, 0.0), right bottom is (1.0, 1.0)
		const Vector2& GetPivot() const { return m_pivot; }
		void SetPivot(const Vector2& pivot);
		//大小
		const Vector2i& GetSize() const { return m_size; }
		void SetSize(int w,int h);
		void SetSize(const Vector2i& size);
		//坐标偏移
		const Vector2i& GetPosition() const { return m_position; }
		void SetPosition(int x,int y);
		void SetPosition(const Vector2i& position);
		//旋转
        void SetRotation(float rotation);
		float GetRotation() const { return m_local_rotation; }
		//缩放
        const Vector2& GetScale() const { return m_local_scale; }
		void SetScale(float x,float y);
        void SetScale(const Vector2& scale);
		//布局
		int GetLeft() { return (int)left; };
		int GetTop() { return (int)top; };
		int GetRight() { return (int)right; };
		int GetBottom() { return (int)bottom; };
		int GetCenterX() { return (int)centerX; };
		int GetCenterY() { return (int)centerY; };
		void SetLeft(int v);
		void SetTop(int v);
		void SetRight(int v);
		void SetBottom(int v);
		void SetCenterX(int v);
		void SetCenterY(int v);
		//空间矩形
		const Rect& GetRect() const { return m_rect; }
		//输入设备处理
        void SetOnTouchDownInside(InputAction func) { m_on_touch_down_inside = func; }
        void SetOnTouchMoveInside(InputAction func) { m_on_touch_move_inside = func; }
        void SetOnTouchUpInside(InputAction func) { m_on_touch_up_inside = func; }
        void SetOnTouchUpOutside(InputAction func) { m_on_touch_up_outside = func; }
        void SetOnTouchDrag(InputAction func) { m_on_touch_drag = func; }
        bool OnTouchDownInside(const Vector2i& pos) const;
        bool OnTouchMoveInside(const Vector2i& pos) const;
        bool OnTouchUpInside(const Vector2i& pos) const;
        bool OnTouchUpOutside(const Vector2i& pos) const;
        bool OnTouchDrag(const Vector2i& pos) const;
	
    protected:
		static const String AppRoot;
        void MarkCanvasDirty() const;
        void ComputeVerticesRectAndMatrix(Rect& rect, Matrix4x4& matrix);
		virtual void FillMeshes(Vector<Mesh2D>& meshes);
		float left{ DEF };
		float top{ DEF };
		float right{ DEF };
		float bottom{ DEF };
		float centerX{ DEF };
		float centerY{ DEF };
		CanvasRenderer* m_canvas{ nullptr };
		Box* m_parent{nullptr};
		Vector<Ref<Box>> m_childs;
		Color m_color{ Color::White() };
		Vector2 m_pivot;
        Vector2i m_size;
        Vector2i m_position;
        Vector2 m_local_scale;
        Rect m_rect;
		float m_local_rotation{ 0.0f };
		float m_rotation{0.0f};
		Vector2 m_scale{ Vector2::One() };
		InputAction m_on_touch_down_inside{ nullptr };
        InputAction m_on_touch_move_inside{ nullptr };
        InputAction m_on_touch_up_inside{ nullptr };
        InputAction m_on_touch_up_outside{ nullptr };
        InputAction m_on_touch_drag{ nullptr };
	};
}
