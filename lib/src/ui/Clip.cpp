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
		this->ComputeVerticesRectAndMatrix(rect, matrix);

		Vector2i innerSize(
			Mathf::Max(m_size.x - m_grid.x - m_grid.width, 0.0f),
			Mathf::Max(m_size.y - m_grid.y - m_grid.height, 0.0f));

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

		Mesh2D mesh;
		mesh.vertices.AddRange({ vs[0], vs[1], vs[2], vs[3] });
		mesh.indices.AddRange({ 0, 1, 2, 0, 2, 3 });
		mesh.view = this;
		mesh.base_view = true;
		meshes.Add(mesh);

		if (m_texture)
		{
			mesh.texture = m_texture;
		}
		else
		{
			mesh.texture = Texture::GetSharedWhiteTexture();
		}

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
