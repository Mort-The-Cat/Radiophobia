#ifndef ASSET_LOADING_CACHE_OPENGL
#define ASSET_LOADING_CACHE_OPENGL

#include "OpenGL_Declarations.h"
#include "Texture_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Mesh_Animator_Declarations.h"
#include "Mesh_Loader.h"

#define LOAD_TEXTURE_SEARCH_CACHE_BIT 1u
#define LOAD_TEXTURE_STORE_CACHE_BIT 2u

#define LOAD_MESH_SEARCH_CACHE_BIT 4u
#define LOAD_MESH_STORE_CACHE_BIT 8u
#define LOAD_MESH_CREATE_BUFFERS_CACHE_BIT 16u

//

#define LOAD_MESH_OBJ_BIT 1u
#define LOAD_MESH_FBX_BIT 2u
#define LOAD_MESH_ANIM_BIT 4u

#define THREADED true

namespace Cache
{
	struct Texture_Cache_Info
	{
		alignas(0) std::string Directory;
		stbi_uc* Pixels = nullptr;
		int Texture_Width, Texture_Height, Texture_Channels;

		Texture Texture;	// This can be taken easily from the cache
	};

	struct Mesh_Cache_Info
	{
		alignas(0) std::string Directory;
		Model_Mesh* Mesh; // This is saved in the cache for later use.

		Model_Vertex_Buffer Vertex_Buffer;

		// However, we must deallocate the mesh data before we remove this item from the mesh cache!!
	};

	struct Animation_Cache_Info
	{
		alignas(0) std::string Directory;
		Mesh_Animation* Animation;
	};

	struct Audio_Cache_Info
	{
		alignas(0) std::string Directory;
		irrklang::ISoundSource* Source;
	};

	struct Text_File_Cache_Info
	{
		alignas(0) std::string Directory;
		std::string Text;
	};

	std::vector<Mesh_Cache_Info> Mesh_Cache;
	std::vector<Texture_Cache_Info> Texture_Cache;
	std::vector<Animation_Cache_Info> Animation_Cache;

	std::vector<Audio_Cache_Info> Audio_Cache;

	std::vector<Text_File_Cache_Info> Text_Cache;

	void Clear_Text_Cache()
	{
		Text_Cache.clear();
	}

	void Clear_Texture_Cache()
	{
		for (size_t W = 0; W < Texture_Cache.size(); W++)
		{
			Texture_Cache[W].Texture.Delete_Texture();
			stbi_image_free(Texture_Cache[W].Pixels);
		}
		Texture_Cache.clear();
	}

	void Clear_Mesh_Cache()
	{
		for (size_t W = 0; W < Mesh_Cache.size(); W++)
		{
			Mesh_Cache[W].Vertex_Buffer.Delete_Buffer();
			delete Mesh_Cache[W].Mesh;
		}
		Mesh_Cache.clear();
	}

	void Remove_From_Mesh_Cache(const char* Directory)
	{
		for(size_t W = 0; W < Mesh_Cache.size(); W++)
			if (strcmp(Directory, Mesh_Cache[W].Directory.c_str()) == 0)
			{
				Mesh_Cache[W].Vertex_Buffer.Delete_Buffer();
				delete Mesh_Cache[W].Mesh;

				break;
			}

		auto Removed_Items = std::remove_if(Mesh_Cache.begin(), Mesh_Cache.end(), [&](Mesh_Cache_Info Cache_Info)->bool { return Cache_Info.Mesh == nullptr; });

		Mesh_Cache.erase(Removed_Items, Mesh_Cache.end());
	}

	void Remove_From_Texture_Cache(const char* Directory)
	{
		for (size_t W = 0; W < Texture_Cache.size(); W++)
		{
			if (strcmp(Directory, Texture_Cache[W].Directory.c_str()) == 0)
			{
				Texture_Cache[W].Texture.Delete_Texture();
				stbi_image_free(Texture_Cache[W].Pixels);
				Texture_Cache[W].Pixels = nullptr;

				break;
			}
		}

		auto Removed_Items = std::remove_if(Texture_Cache.begin(), Texture_Cache.end(), [&](Texture_Cache_Info Cache_Info)->bool { return Cache_Info.Pixels == nullptr; });

		Texture_Cache.erase(Removed_Items, Texture_Cache.end());
	}

	void Remove_From_Audio_Cache(const char* Directory)
	{
		for (size_t W = 0; W < Audio_Cache.size(); W++)
		{
			if (strcmp(Directory, Audio_Cache[W].Directory.c_str()) == 0)
			{
				Audio_Cache[W].Source->drop();
				Audio_Cache[W].Source = nullptr;
			}
		}

		auto Removed_Items = std::remove_if(Audio_Cache.begin(), Audio_Cache.end(), [&](Audio_Cache_Info Cache_Info)->bool { return Cache_Info.Source == nullptr; });
		Audio_Cache.erase(Removed_Items, Audio_Cache.end());
	}

	void Remove_From_Animation_Cache(Mesh_Animation* Item)
	{
		auto Removed_Items = std::remove_if(Animation_Cache.begin(), Animation_Cache.end(), [&](Animation_Cache_Info Cache_Info)->bool { return Cache_Info.Animation == Item; });

		Animation_Cache.erase(Removed_Items, Animation_Cache.end());

		delete Item;
	}

	void Clear_Animation_Cache()
	{
		for (size_t W = 0; W < Animation_Cache.size(); W++)
			delete Animation_Cache[W].Animation;
		Animation_Cache.clear();
	}

	void Clear_Audio_Cache()
	{
		for (size_t W = 0; W < Audio_Cache.size(); W++)
		{
			Audio_Cache[W].Source->drop();
		}
		Audio_Cache.clear();
	}

	bool Search_Texture_Cache(const char* Directory, Texture_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Texture_Cache.size(); W++)
		{
			if (strcmp(Directory, Texture_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Texture_Cache[W];
				return true;
			}
		}
		return false;
	}

	bool Search_Text_Cache(const char* Directory, Text_File_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Text_Cache.size(); W++)
		{
			if (strcmp(Directory, Text_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Text_Cache[W];
				return true;
			}
		}

		return false;
	}

	bool Search_Mesh_Cache(const char* Directory, Mesh_Cache_Info* Target_Info, unsigned char Flags)
	{
		for (size_t W = 0; W < Mesh_Cache.size(); W++)
		{
			if (strcmp(Directory, Mesh_Cache[W].Directory.c_str()) == 0 && ((bool)(Flags & LOAD_MESH_ANIM_BIT) == (Mesh_Cache[W].Vertex_Buffer.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)))
			{
				// Even if the directory is the same, whether or not the model was loaded with the LOAD_MESH_ANIM_BIT flag massively changes the vertex data
				// hence we need to check for that

				*Target_Info = Mesh_Cache[W];
				return true;
			}
		}
		return false;
	}

	bool Search_Animation_Cache(const char* Directory, Animation_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Animation_Cache.size(); W++)
		{
			if (strcmp(Directory, Animation_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Animation_Cache[W];
				return true;
			}
		}
		return false;
	}

	bool Search_Audio_Cache(const char* Directory, Audio_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Audio_Cache.size(); W++)
		{
			if (strcmp(Directory, Audio_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Audio_Cache[W];
				return true;
			}
		}
		return false;
	}
}

void Load_New_Texture(std::string Directory, Cache::Texture_Cache_Info* Target_Info)
{
	Target_Info->Texture.Create_Texture();

	stbi_set_flip_vertically_on_load(false);

	Target_Info->Pixels = stbi_load(Directory.c_str(), &Target_Info->Texture_Width, &Target_Info->Texture_Height, &Target_Info->Texture_Channels, 4u);

	if (Target_Info->Pixels)
	{
		auto Channel_Flag = Target_Info->Texture_Channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Target_Info->Texture_Width, Target_Info->Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Target_Info->Pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		Throw_Error(" >> Unable to load texture!");

	Target_Info->Directory = Directory;
}

template<bool Is_Threaded = false>
void Push_Merged_Specular_Reflectivity(const char* T_1, const char* T_2, const char* Material_Name)
{
	Cache::Texture_Cache_Info Final_Texture;
	std::array<Cache::Texture_Cache_Info, 2> Textures;
	std::array<const char*, 2> Directories = { T_1, T_2 };

	for (size_t W = 0; W < 2; W++)
		Textures[W].Pixels = stbi_load(Directories[W], &Textures[W].Texture_Width, &Textures[W].Texture_Height, &Textures[W].Texture_Channels, STBI_rgb_alpha);

	size_t Total_Size = Textures[0].Texture_Width * Textures[0].Texture_Height * 4;

	Final_Texture.Pixels = (stbi_uc*)malloc(Total_Size);

	for (size_t W = 0; W < Total_Size; W += 4)
	{
		((unsigned char*)Final_Texture.Pixels)[W] = (unsigned char)Textures[0].Pixels[W];
		((unsigned char*)Final_Texture.Pixels)[W + 1] = 255; // (unsigned char)Textures[1].Pixels[W];
		((unsigned char*)Final_Texture.Pixels)[W + 2] = 128;
		((unsigned char*)Final_Texture.Pixels)[W + 3] = 128;
	}

	Final_Texture.Texture_Channels = Textures[0].Texture_Channels;
	Final_Texture.Texture_Width = Textures[0].Texture_Width;
	Final_Texture.Texture_Height = Textures[0].Texture_Height;

	Final_Texture.Directory = Material_Name;

	for (size_t W = 0; W < 2; W++)
		stbi_image_free(Textures[W].Pixels);

	if constexpr (Is_Threaded)
		Context_Interface::Request_Context();

	Final_Texture.Texture.Create_Texture();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Final_Texture.Texture_Width, Final_Texture.Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Final_Texture.Pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	if constexpr (Is_Threaded)
	{
		Context_Interface::Free_Context();
		Context_Interface::Loading_Progress++;
	}

	Cache::Texture_Cache.push_back(Final_Texture);
}

template<bool Is_Threaded = false>
void Push_Merged_Material(const char* T_1, const char* T_2, const char* T_3, const char* Material_Name)
{
	Cache::Texture_Cache_Info Final_Texture;
	std::array<Cache::Texture_Cache_Info, 3> Textures;
	std::array<const char*, 3> Directories = { T_1, T_2, T_3 };

	if(Cache::Search_Texture_Cache(Material_Name, &Final_Texture))		// The material already exists...
		return;

	for (size_t W = 0; W < 3; W++)
		if (Directories[W] != nullptr)
			Textures[W].Pixels = stbi_load(Directories[W], &Textures[W].Texture_Width, &Textures[W].Texture_Height, &Textures[W].Texture_Channels, STBI_rgb_alpha);
		else
		{
			Textures[W].Pixels = (stbi_uc*)malloc(Textures[0].Texture_Width * Textures[0].Texture_Height * Textures[0].Texture_Channels);
			memset(Textures[W].Pixels, 0u, Textures[0].Texture_Width * Textures[0].Texture_Height * Textures[0].Texture_Channels);
		}

	size_t Total_Size = Textures[0].Texture_Width * Textures[0].Texture_Height * 4;

	Final_Texture.Pixels = (stbi_uc*)malloc(Total_Size);

	for (size_t W = 0; W < Total_Size; W += 4)
	{
		((char*)Final_Texture.Pixels)[W] = (char)Textures[0].Pixels[W];
		((char*)Final_Texture.Pixels)[W + 1] = (char)Textures[2].Pixels[W + 2];
		((char*)Final_Texture.Pixels)[W + 2] = (char)Textures[2].Pixels[W + 1];
		((char*)Final_Texture.Pixels)[W + 3] = (char)Textures[2].Pixels[W + 0];
	}

	Final_Texture.Texture_Channels = Textures[0].Texture_Channels;
	Final_Texture.Texture_Width = Textures[0].Texture_Width;
	Final_Texture.Texture_Height = Textures[0].Texture_Height;

	Final_Texture.Directory = Material_Name;

	for (size_t W = 0; W < 3; W++)
		stbi_image_free(Textures[W].Pixels);

	//

	if constexpr (Is_Threaded)
		Context_Interface::Request_Context();

	//

	Final_Texture.Texture.Create_Texture();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Final_Texture.Texture_Width, Final_Texture.Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Final_Texture.Pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	Cache::Texture_Cache.push_back(Final_Texture);

	//

	if constexpr (Is_Threaded)
	{
		Context_Interface::Free_Context();
		Context_Interface::Loading_Progress++;
	}
}

template<bool Is_Threaded = false>
Cache::Texture_Cache_Info Pull_Texture(const char* Texture_Directory)
{
	Cache::Texture_Cache_Info Cache_Info;

	//if (Flags & LOAD_TEXTURE_SEARCH_CACHE_BIT)
	//{
	if (Cache::Search_Texture_Cache(Texture_Directory, &Cache_Info))
	{
		if constexpr (Is_Threaded)
			Context_Interface::Loading_Progress++;

		return Cache_Info;
	}
	//}

	if constexpr (Is_Threaded)
		Context_Interface::Request_Context();

	Load_New_Texture(Texture_Directory, &Cache_Info);

	if constexpr (Is_Threaded)
	{
		Context_Interface::Free_Context();
		Context_Interface::Loading_Progress++;
	}

	Cache::Texture_Cache.push_back(Cache_Info);
	return Cache_Info;
}

//

template<bool Is_Threaded = false>
Cache::Animation_Cache_Info Pull_Animation(const char* Directory)
{
	Cache::Animation_Cache_Info Cache_Info;

	if (Cache::Search_Animation_Cache(Directory, &Cache_Info))
	{
		if constexpr (Is_Threaded)
			Context_Interface::Loading_Progress++;

		return Cache_Info;
	}

	Cache_Info.Animation = new Mesh_Animation();

	Load_Animation_File(Directory, Cache_Info.Animation);

	if constexpr (Is_Threaded)
		Context_Interface::Loading_Progress++;

	Cache_Info.Directory = Directory;

	Cache::Animation_Cache.push_back(Cache_Info);
	return Cache_Info;
}

//

template<bool Is_Threaded = false>
Cache::Mesh_Cache_Info Pull_Mesh(const char* Directory, unsigned char Flags = LOAD_MESH_OBJ_BIT)
{
	Cache::Mesh_Cache_Info Cache_Info;

	if (Cache::Search_Mesh_Cache(Directory, &Cache_Info, Flags))
	{
		if (Flags & LOAD_MESH_ANIM_BIT) // If this mesh is an anim mesh, we need it to have a unique vertex buffer
		{
			Cache_Info.Mesh = new Model_Mesh(Cache_Info.Mesh);	// This should create a copy thereof?
			Cache_Info.Vertex_Buffer.Mesh = Cache_Info.Mesh;	// Assign mesh to vertex buffer appropriately

			if constexpr (Is_Threaded)
				Context_Interface::Request_Context();

			Cache_Info.Vertex_Buffer.Buffer_Storage_Hint = GL_DYNAMIC_DRAW;

			Cache_Info.Vertex_Buffer.Create_Buffer();
			Cache_Info.Vertex_Buffer.Bind_Buffer();
			Cache_Info.Vertex_Buffer.Update_Buffer();

			if constexpr (Is_Threaded)
				Context_Interface::Free_Context();
		}

		if constexpr (Is_Threaded)
			Context_Interface::Loading_Progress++;

		return Cache_Info;
	}

	Cache_Info.Mesh = new Model_Mesh(); // This allocates the memory that is used by the load_mesh_obj function

	if (Flags & LOAD_MESH_ANIM_BIT)
	{
		Cache_Info.Vertex_Buffer.Buffer_Storage_Hint = GL_DYNAMIC_DRAW;
		Flags -= LOAD_MESH_ANIM_BIT;
	}

	switch (Flags)
	{
	case LOAD_MESH_OBJ_BIT:
	default:
		Load_Mesh_Obj(Directory, Cache_Info.Mesh, Cache_Info.Vertex_Buffer.Buffer_Storage_Hint == GL_DYNAMIC_DRAW);
		break;
	case LOAD_MESH_FBX_BIT:
		Load_Mesh_Fbx(Directory, Cache_Info.Mesh);
		break;
	}

	Cache_Info.Directory = Directory;
	Cache_Info.Vertex_Buffer.Mesh = Cache_Info.Mesh;

	if (Cache_Info.Vertex_Buffer.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)
	{
		Cache::Mesh_Cache.push_back(Cache_Info);
		return Pull_Mesh<Is_Threaded>(Directory, Flags | LOAD_MESH_ANIM_BIT);
	}

	if constexpr (Is_Threaded)
		Context_Interface::Request_Context();

	Cache_Info.Vertex_Buffer.Create_Buffer();

	//uint16_t Buffer_Storage_Hints[2] = { GL_STATIC_DRAW, GL_DYNAMIC_DRAW };

	//Cache_Info.Vertex_Buffer.Buffer_Storage_Hint = Buffer_Storage_Hints[(bool)(Flags & LOAD_MESH_ANIM_BIT)];

	// This changes the buffer storage hint to allow for better optimisation

	Cache_Info.Vertex_Buffer.Bind_Buffer();
	Cache_Info.Vertex_Buffer.Update_Buffer();

	if constexpr (Is_Threaded)
	{
		Context_Interface::Loading_Progress++;
		Context_Interface::Free_Context();
	}

	Cache::Mesh_Cache.push_back(Cache_Info);
	return Cache_Info;
}

//

Cache::Audio_Cache_Info Pull_Audio(const char* Directory)
{
	Cache::Audio_Cache_Info Cache_Info;

	if (Cache::Search_Audio_Cache(Directory, &Cache_Info))
	{
		return Cache_Info;
	}

	Cache_Info.Directory = Directory;
	Cache_Info.Source = Sound_Engine->addSoundSourceFromFile(Directory, irrklang::ESM_AUTO_DETECT, true);

	Cache::Audio_Cache.push_back(Cache_Info);

	return Cache_Info;
}

Cache::Text_File_Cache_Info Pull_Text(const char* Directory, unsigned char Cache_Value = false)
{
	Cache::Text_File_Cache_Info Cache_Info;

	if (Cache::Search_Text_Cache(Directory, &Cache_Info))
		return Cache_Info;

	Cache_Info.Directory = Directory;
	Cache_Info.Text = Get_File_Contents(Directory);

	if(Cache_Value)
		Cache::Text_Cache.push_back(Cache_Info);

	return Cache_Info;
}

#endif