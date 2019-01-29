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

#include "Clip.h"
#include "CanvasRenderer.h"
#include "graphics/Texture.h"
#include "math/Mathf.h"
namespace Viry3D
{
	Clip::Clip()
    {
    
    }
    
	Clip::~Clip()
    {
    
    }
	void Clip::SetSkin(const String& skin)
	{
		m_skin = skin;

		Texture::LoadTexture2DFromFile( + "/texture/ui/button.png", FilterMode::Linear, SamplerAddressMode::ClampToEdge, false, false);
	}
	void Clip::FillMeshes(Vector<Mesh2D>& meshes)
	{
		Rect rect;
		Matrix4x4 matrix;
		float x_, y_, w_, h_ = 0.0f;
		this->ComputeVerticesRectAndMatrix(rect, matrix);
		Mesh2D mesh;
		if (m_texture)
		{
			mesh.texture = m_texture;
		}
		else
		{
			mesh.texture = Texture::GetSharedWhiteTexture();
		}
		if (m_grid.x > 0 && m_grid.y > 0 && m_grid.width > 0 && m_grid.height > 0) {//全九宫格  4
			Vertex vs[16];
			Memory::Zero(&vs[0], sizeof(vs));
			x_ = rect.x;
			y_ = rect.y;
			w_ = rect.x + m_grid.x;
			h_ = rect.y - m_grid.y;
			vs[0].vertex = Vector3(x_, y_, 0);
			vs[1].vertex = Vector3(x_, h_, 0);
			vs[2].vertex = Vector3(w_, h_, 0);
			vs[3].vertex = Vector3(w_, y_, 0);
			vs[0].color = m_color;
			vs[1].color = m_color;
			vs[2].color = m_color;
			vs[3].color = m_color;

			vs[0].uv = Vector2(0.0f, 0.0f);
			vs[1].uv = Vector2(0.0f, 0);

			x_ = w_;
			y_ = y_;
			w_ = x_ + (m_size.x - m_grid.x - m_grid.width);
			h_ = h_;

			//vs[3].vertex = Vector3(x_, y_, 0);
			//vs[2].vertex = Vector3(x_, h_, 0);
			vs[4].vertex = Vector3(x_+ w_, h_, 0);
			vs[5].vertex = Vector3(x_ + w_, y_, 0);
			vs[4].color = m_color;
			vs[5].color = m_color;

			x_ = w_;
			y_ = y_;
			w_ = x_ + m_size.x;
			h_ = h_;

			//vs[5].vertex = Vector3(x_, y_, 0);
			//vs[4].vertex = Vector3(x_, h_, 0);
			vs[6].vertex = Vector3(x_ + w_, h_, 0);
			vs[7].vertex = Vector3(x_ + w_, y_, 0);
			vs[6].color = m_color;
			vs[7].color = m_color;

			x_ = rect.x;
			y_ = rect.y - m_grid.y;
			w_ = x_ + m_grid.x;
			h_ = y_ - (m_size.y - m_grid.y - m_grid.height);

			//vs[5].vertex = Vector3(x_, y_, 0);
			//vs[4].vertex = Vector3(x_, h_, 0);
			vs[8].vertex = Vector3(x_, h_, 0);
			vs[9].vertex = Vector3(w_ + h_, y_, 0);
			vs[8].color = m_color;
			vs[9].color = m_color;

			x_ = w_;
			w_ = x_ + (m_size.x - m_grid.x - m_grid.width);
			vs[10].vertex = Vector3(w_, y_, 0);
			vs[10].color = m_color;

			x_ = w_;
			w_ = x_ + m_size.x;

			vs[11].vertex = Vector3(w_, y_, 0);
			vs[11].color = m_color;

			x_ = rect.x;
			y_ = rect.y - m_size.y;
			w_ = x_ + m_grid.x;

			vs[12].vertex = Vector3(x_, y_, 0);
			vs[13].vertex = Vector3(w_, y_, 0);
			vs[12].color = m_color;
			vs[13].color = m_color;

			x_ = rect.x+m_size.x-m_grid.width;

			vs[14].vertex = Vector3(x_, y_, 0);
			vs[14].color = m_color;

			x_ = rect.x + m_size.x;

			vs[15].vertex = Vector3(x_, y_, 0);
			vs[15].color = m_color;


		}
		else
		{
			Vertex vs[4];
			Memory::Zero(&vs[0], sizeof(vs));
			vs[0].vertex = Vector3(rect.x, rect.y, 0);
			vs[1].vertex = Vector3(rect.x, rect.y - rect.height, 0);
			vs[2].vertex = Vector3(rect.x + rect.width, rect.y - rect.height, 0);
			vs[3].vertex = Vector3(rect.x + rect.width, rect.y, 0);
			vs[0].color = m_color;
			vs[1].color = m_color;
			vs[2].color = m_color;
			vs[3].color = m_color;
			vs[0].uv = Vector2(0, 0);
			vs[1].uv = Vector2(0, 1);
			vs[2].uv = Vector2(1, 1);
			vs[3].uv = Vector2(1, 0);

			for (int i = 0; i < 4; ++i)
			{
				vs[i].vertex = matrix.MultiplyPoint3x4(vs[i].vertex);
			}

			mesh.vertices.AddRange({ vs[0], vs[1], vs[2], vs[3] });
			mesh.indices.AddRange({ 0, 1, 2, 0, 2, 3 });
			mesh.view = this;
			mesh.base_view = true;
			meshes.Add(mesh);
		}
		/*else if (m_grid.x > 0 && m_grid.y > 0 && (m_grid.width > 0 || m_grid.height > 0)) {//左右 上或下 3
			Vertex vs[16];
			Memory::Zero(&vs[0], sizeof(vs));

			m_grid = m_grid.y > 0 ? m_grid.y : m_grid.height;

			x_ = rect.x;
			y_ = rect.y;
			w_ = rect.x + m_grid.x;
			h_ = rect.y - m_grid.y;



			vs[0].vertex = Vector3(x_, y_, 0);
			vs[1].vertex = Vector3(x_, h_, 0);
			vs[2].vertex = Vector3(w_, h_, 0);
			vs[3].vertex = Vector3(w_, y_, 0);


		}
		else if (m_grid.width > 0 && m_grid.height > 0 > 0 && (m_grid.x > 0 || m_grid.y > 0)) {//上下 左或右 3

		}
		else if ((m_grid.x > 0 && m_grid.y > 0) || (m_grid.width > 0 && m_grid.height > 0)) {//左上或右下 2
		
		}
		else if ((m_grid.x > 0 && m_grid.width > 0) || (m_grid.y > 0 && m_grid.height > 0)) {//左右或上下 2

		}
		else if (m_grid.x > 0 || m_grid.width > 0 || m_grid.y > 0 || m_grid.height > 0) {//左或右或上或下 1

		}*/

		

		if (mesh.texture == Texture::GetSharedWhiteTexture() || mesh.texture == Texture::GetSharedBlackTexture())
		{
			mesh.vertices[0].uv = Vector2(1.0f / 3, 1.0f / 3);
			mesh.vertices[1].uv = Vector2(1.0f / 3, 2.0f / 3);
			mesh.vertices[2].uv = Vector2(2.0f / 3, 2.0f / 3);
			mesh.vertices[3].uv = Vector2(2.0f / 3, 1.0f / 3);
		}
	}
	/*
    void Clip::SetTexture(const Ref<Texture>& texture)
    {
        m_texture = texture;
        this->MarkCanvasDirty();
    }

    void Clip::FillSelfMeshes(Vector<ViewMesh>& meshes)
    {
        Box::FillSelfMeshes(meshes);

        ViewMesh& mesh = meshes[meshes.Size() - 1];

        if (m_texture)
        {
            mesh.texture = m_texture;
        }
        else
        {
            mesh.texture = Texture::GetSharedWhiteTexture();
        }

        if (mesh.texture == Texture::GetSharedWhiteTexture() ||
            mesh.texture == Texture::GetSharedBlackTexture())
        {
            mesh.vertices[0].uv = Vector2(1.0f / 3, 1.0f / 3);
            mesh.vertices[1].uv = Vector2(1.0f / 3, 2.0f / 3);
            mesh.vertices[2].uv = Vector2(2.0f / 3, 2.0f / 3);
            mesh.vertices[3].uv = Vector2(2.0f / 3, 1.0f / 3);
        }
    }*/
}
