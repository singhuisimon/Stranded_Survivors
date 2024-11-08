/**
 * @file Graphics_Manager.cpp
 * @brief Implements the Graphics_Manager class methods.
 * @author Chua Wen Bin Kenny (99.64%), Liliana Hanawardani (0.359%)
 * @date September 18, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Graphics_Manager.h" 
#include "../Utility/Path_Helper.h" // For file path resolution
#include "Assets_Manager.h"

// FOR TESTING (texture loading)
#define STB_IMAGE_IMPLEMENTATION 
#include "STB/stb_image.h"  // For loading textures/sprites 

namespace lof {

    std::unique_ptr<Graphics_Manager> Graphics_Manager::instance;
    std::once_flag Graphics_Manager::once_flag;

    // Constructor
    Graphics_Manager::Graphics_Manager() {
        set_type("Graphics_Manager");
        m_is_started = false;
        render_mode = GL_FILL;
    }

    // Destructor
    Graphics_Manager::~Graphics_Manager() {
        if (is_started()) {
            shut_down();
        }
    }

    // Singleton
    Graphics_Manager& Graphics_Manager::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Graphics_Manager);
            });
        return *instance;
    }

    // Initialize the manager by setting up the viewport and loading assets such
    // as shader programs and models
    int Graphics_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // Set framebuffer with color (Background color)
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Set viewport position and dimensions
        GLuint scr_width = SM.get_scr_width();
        GLuint scr_height = SM.get_scr_height();
        glViewport(0, 0, scr_width, scr_height); 

        // Set up default render mode 
        render_mode = GL_FILL;



        std::string vertex_obj_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_obj.vert");
        std::string fragment_obj_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_obj.frag");
        std::string vertex_debug_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_debug.vert");
        std::string fragment_debug_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_debug.frag");
        std::string vertex_font_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_font.vert");
        std::string fragment_font_path = ASM.get_full_path(ASM.SHADER_PATH, "lack_of_oxygen_font.frag");

        std::vector<std::pair<std::string, std::string>> shader_files{ // vertex & fragment shader files
            std::make_pair(vertex_obj_path, fragment_obj_path),
            std::make_pair(vertex_debug_path, fragment_debug_path),
            std::make_pair(vertex_font_path, fragment_font_path)
        };



        if (!ASM.load_shader_programs(shader_files)) {
            LM.write_log("Graphics_Manager::start_up(): Failed to load shader programs");
            return -1;
        }
        else {
            LM.write_log("Graphics_Manager::start_up(): Succesfully added shader programs.");
        }


        std::string mesh_path = ASM.get_full_path(ASM.MODEL_PATH, "models.msh");
        std::string texture_path = ASM.get_full_path(ASM.TEXTURE_PATH, "Texture_Names.txt");
        std::string animation_path = ASM.get_full_path(ASM.TEXTURE_PATH, "Prisoner_Atlas.txt");
        std::string font_path = ASM.get_full_path(ASM.FONT_PATH, "Fonts.txt");

        if (!add_model(mesh_path)) {
            LM.write_log("Graphics_Manager::start_up(): Failed to add models");
            return -2;
        }

        std::vector<std::string> texture_names;
        if (!ASM.load_all_textures(texture_path, texture_names)) {
            LM.write_log("Graphics_Manager: Failed to load texture list");
            return -3;
        }

        if (!add_textures(texture_names)) {
            LM.write_log("Graphics_Manager: Failed to create textures");
            return -3;
        }

        // Add animations
        if (!add_animations(animation_path.c_str())) { 
            LM.write_log("Fail to add animations.");
            return -4;
        }

        // Add fonts
        if (!add_fonts(font_path.c_str())) {
            LM.write_log("Fail to add fonts.");
            std::cout << "AAAAA" << std::endl;
            return -5;
        }

        m_is_started = true;
        return 0;
    }

    // Shut down the manager when game is shutting down
    void Graphics_Manager::shut_down() {
        if (!is_started()) {
            return; // Not started
        }

        // Free the data storages
        //shader_program_storage.clear();
        ASM.unload_shader_programs();
        model_storage.clear();
        texture_storage.clear();
        animation_storage.clear();

       

        m_is_started = false;
    }

    // Update rendering details according to inputs
    void Graphics_Manager::update() { 
        // Change render mode with 1 (FILL), 2 (LINE), 3 (POINT) 
        if (IM.is_key_held(GLFW_KEY_1)) {
            LM.write_log("Graphics_Manager::update(): '1' key pressed, render mode is now FILL.");
            render_mode = GL_FILL;
        } else if (IM.is_key_held(GLFW_KEY_2)) {
            LM.write_log("Graphics_Manager::update(): '2' key pressed, render mode is now LINE.");
            render_mode = GL_LINE;
        } else if (IM.is_key_held(GLFW_KEY_3)) {
            LM.write_log("Graphics_Manager::update(): '3' key pressed, render mode is now POINT.");
            render_mode = GL_POINT; 
        }

        // Toggle debug mode using 'B" or 'N'
        if (IM.is_key_held(GLFW_KEY_B)) {
            LM.write_log("Graphics_Manager::update(): 'B' key pressed, Debug Mode is now ON.");
            is_debug_mode = GL_TRUE;
        }
        else if (IM.is_key_held(GLFW_KEY_N)) {
            LM.write_log("Graphics_Manager::update(): 'N' key pressed, Debug Mode is now OFF.");
            is_debug_mode = GL_FALSE;
        }

        // Toggle free camera mode using 'Z' and 'X'
        if (IM.is_key_held(GLFW_KEY_Z)) {
            LM.write_log("Graphics_Manager::update(): 'Z' key pressed, Free Camera enabled.");
            camera.is_free_cam = GL_TRUE;
        }
        else if (IM.is_key_held(GLFW_KEY_X)) {
            LM.write_log("Graphics_Manager::update(): 'X' key pressed, Free Camera disabled.");
            camera.is_free_cam = GL_FALSE;
        }

    }



    GLboolean Graphics_Manager::add_model(const std::string& file_name) {
        if (!ASM.load_model_data(file_name)) {
            LM.write_log("Graphics_Manager: Failed to load model data");
            return GL_FALSE;
        }

        for (const auto& pair : ASM.model_storage) {
            const std::string& model_name = pair.first;
            const Assets_Manager::ModelData& modelData = pair.second;

            // Skip if model already exists in Graphics_Manager
            if (model_storage.find(model_name) != model_storage.end()) {
                continue;
            }

            // Create OpenGL resources
            Model mdl{};
            GLuint vbo_hdl{}, vaoid{}, ebo_hdl{};
            glCreateVertexArrays(1, &vaoid);

            const size_t data_size = (model_name == "square") ?
                sizeof(Assets_Manager::TexVtxData) : sizeof(glm::vec2);

            if (model_name == "square") {
                glCreateBuffers(1, &vbo_hdl);
                glNamedBufferStorage(vbo_hdl,
                    sizeof(Assets_Manager::TexVtxData) * modelData.texVtxArr.size(),
                    modelData.texVtxArr.data(),
                    GL_DYNAMIC_STORAGE_BIT);

                // Set up texture coordinates (attribute index 1, binding point 7)
                glEnableVertexArrayAttrib(vaoid, 1);
                glVertexArrayVertexBuffer(vaoid, 7, vbo_hdl,
                    sizeof(glm::vec2),
                    sizeof(Assets_Manager::TexVtxData));
                glVertexArrayAttribFormat(vaoid, 1, 2, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vaoid, 1, 7);
            }
            else {
                glCreateBuffers(1, &vbo_hdl);
                glNamedBufferStorage(vbo_hdl,
                    data_size * modelData.posVtx.size(),
                    modelData.posVtx.data(),
                    GL_DYNAMIC_STORAGE_BIT);
            }

            // Set up position attributes (attribute index 0, binding point 6)
            glEnableVertexArrayAttrib(vaoid, 0);
            glVertexArrayVertexBuffer(vaoid, 6, vbo_hdl, 0, data_size);
            glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vaoid, 0, 6);

            // Set up index buffer
            glCreateBuffers(1, &ebo_hdl);
            glNamedBufferStorage(ebo_hdl,
                sizeof(GLushort) * modelData.vtxIdx.size(),
                modelData.vtxIdx.data(),
                GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayElementBuffer(vaoid, ebo_hdl);

            // Unbind VAO
            glBindVertexArray(0);

            // Store the model
            mdl.vaoid = vaoid;
            mdl.primitive_type = modelData.primitiveType;
            mdl.draw_cnt = static_cast<GLuint>(modelData.vtxIdx.size());
            model_storage[model_name] = mdl;

            LM.write_log("Graphics_Manager: Created GPU resources for model %s", model_name.c_str());
    }

        LM.write_log("Graphics_Manager: All models successfully created and stored.");
        return GL_TRUE;
    }



    GLboolean Graphics_Manager::add_textures(const std::vector<std::string>& texture_names) {
        for (const auto& tex_name : texture_names) {
            // Construct the full path
            std::string tex_filepath = ASM.get_full_path(ASM.TEXTURE_PATH, tex_name + ".png");

            // Add debug logging
            //LM.write_log("Assets_Manager: Providing texture path: %s", tex_filepath.c_str());
            LM.write_log("Graphics_Manager: Loading texture from %s", tex_filepath.c_str());

            // Load texture data
            int width{ 0 }, height{ 0 }, channels{ 0 };
            stbi_set_flip_vertically_on_load(1);
            unsigned char* tex_data = stbi_load(tex_filepath.c_str(), &width, &height, &channels, 4);

            // Add debug logging for dimensions
            LM.write_log("Graphics_Manager: Texture dimensions: %dx%d with %d channels", width, height, channels);

            // Create and initialize texture object
            GLuint tex_id{};
            if (tex_data) {
                glGenTextures(1, &tex_id);
                glBindTexture(GL_TEXTURE_2D, tex_id);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
                glBindTexture(GL_TEXTURE_2D, 0);

                stbi_image_free(tex_data);

                // Add debug logging
                LM.write_log("Graphics_Manager: Created texture with ID %u for %s", tex_id, tex_name.c_str());
            }
            else {
                stbi_image_free(tex_data);
                LM.write_log("Graphics_Manager: Failed to load texture data for %s", tex_name.c_str());
                return GL_FALSE;
            }

            // Add texture object id to texture storage
            texture_storage[tex_name] = tex_id;
        }

        LM.write_log("Graphics_Manager: Added %d textures to storage", texture_storage.size());
        return GL_TRUE;
    }



    // Add fonts into the font storage
    GLboolean Graphics_Manager::add_fonts(std::string const& file_name) {
        // Create filepath to access the fonts in fonts.txt file
        std::vector<std::string> font_names;
        if (!ASM.read_font_list(file_name, font_names)) {
            return GL_FALSE;
        }

        for (const auto& font_name : font_names) {
            FT_Library font_type;
            FT_Face face;

            if (!ASM.load_fonts(font_name, font_type, face)) {
                continue;
            }

            // Set glyph size to load
            FT_Set_Pixel_Sizes(face, DEFAULT_GLYPH_WIDTH, DEFAULT_GLYPH_HEIGHT);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Load first 128 characters of ASCII set
            std::map<GLchar, Character> characters_set;
            Font new_font{};

            for (unsigned char ch = 0; ch < 128; ++ch) {
                // Load character glyph
                if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
                    LM.write_log("Failed to load Glyph");
                    continue;
                }

                // Generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                // Set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Allocate memory for texture data and upload it
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                    face->glyph->bitmap.width, face->glyph->bitmap.rows,
                    0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

                // Store character information
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                characters_set.insert(std::pair<char, Character>(ch, character));
            }

            // Create and configure VAO/VBO for rendering
            GLuint vbo_hdl, vaoid;
            glGenVertexArrays(1, &vaoid);
            glGenBuffers(1, &vbo_hdl);
            glBindVertexArray(vaoid);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_hdl);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            // Store font data
            new_font.vaoid = vaoid;
            new_font.vboid = vbo_hdl;
            new_font.characters = characters_set;

            // Add to storage
            font_storage[font_name] = new_font;
            LM.write_log("Font %s successfully added.", font_name.c_str());

            glBindTexture(GL_TEXTURE_2D, 0);
            FT_Done_Face(face);
            FT_Done_FreeType(font_type);
        }

        LM.write_log("All fonts successfully created and stored.");
        return GL_TRUE;
    }

    GLboolean Graphics_Manager::add_animations(const std::string& file_name) {
        return ASM.load_animations(file_name);
    }

    // Return reference to shader program storage
   // Graphics_Manager::SHADERS& Graphics_Manager::get_shader_program_storage() { return shader_program_storage; }

    // Return reference to model storage
    Graphics_Manager::MODELS& Graphics_Manager::get_model_storage() { return model_storage; }

    // Return reference to texture storage
    Graphics_Manager::TEXTURES& Graphics_Manager::get_texture_storage() { return texture_storage; }

    // Return reference to animation storage
    Graphics_Manager::ANIMATIONS& Graphics_Manager::get_animation_storage() { return animation_storage; } 

    // Return reference to font storage
    Graphics_Manager::FONTS& Graphics_Manager::get_font_storage() { return font_storage; }

    // Return state of current render mode
    GLenum& Graphics_Manager::get_render_mode() { return render_mode; }

    // Return state of current debig mode
    GLboolean& Graphics_Manager::get_debug_mode() { return is_debug_mode; }

    // Return reference to camera
    Graphics_Manager::Camera2D& Graphics_Manager::get_camera() { return camera; }

    GLboolean Graphics_Manager::compile_shader(std::vector<std::pair<GLenum, std::string>> shader_files, Assets_Manager::ShaderProgram& shader) {
        // Read each shader file details such as shader type and file path
        for (auto& file : shader_files) {
            // Check if file's state is good for reading
            std::string shader_source;
            if (!ASM.read_shader_file(file.second, shader_source)) {
                LM.write_log("Graphics_Manager::compile_shader(): File %s has error.", file.second.c_str());
                return GL_FALSE;
            }
            LM.write_log("Graphics_Manager::compile_shader(): File %s is good for reading.", file.second.c_str());

            // Create shader program
            if (shader.program_handle <= 0) {
                shader.program_handle = glCreateProgram();
                if (shader.program_handle == 0) {
                    LM.write_log("Graphics_Manager::compile_shader(): Cannot create program handle");
                    return GL_FALSE;
                }
                LM.write_log("Graphics_Manager::compile_shader(): Program handle %u created", shader.program_handle);
            }

            // Create shader object and load shader code with it
            GLuint shader_obj = 0;
            if (file.first == GL_VERTEX_SHADER) {
                shader_obj = glCreateShader(GL_VERTEX_SHADER);
            }
            else if (file.first == GL_FRAGMENT_SHADER) {
                shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
            }
            else {
                LM.write_log("Graphics_Manager::compile_shader(): Invalid shader type.");
                return GL_FALSE;
            }
            const GLchar* shader_code[] = { shader_source.c_str() };
            glShaderSource(shader_obj, 1, shader_code, NULL);

            // Compile and check if successful
            glCompileShader(shader_obj);
            GLint compile_status;
            glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &compile_status);
            if (compile_status == GL_FALSE) {
                LM.write_log("Graphics_Manager::compile_shader(): Shader from file %s compilation fail.", file.second.c_str());

                return GL_FALSE;
            }
            else {
                glAttachShader(shader.program_handle, shader_obj);
                LM.write_log("Graphics_Manager::compile_shader(): Shader from file %s compilation successful.", file.second.c_str());
            }
        }

        // Check if compiled shaders in shader program is linked
        if (shader.link_status == GL_FALSE) {
            glLinkProgram(shader.program_handle);
            GLint link_status;
            glGetProgramiv(shader.program_handle, GL_LINK_STATUS, &link_status);
            if (link_status == GL_FALSE) {
                LM.write_log("Graphics_Manager::compile_shader(): Compiled shaders failed to link.");
                return GL_FALSE;
            }
            shader.link_status = GL_TRUE;
            LM.write_log("Graphics_Manager::compile_shader(): Compiled shaders are linked successfully.");
        }

        // Check if the program created can be executed in current OpenGL state
        glValidateProgram(shader.program_handle);
        GLint validate_status;
        glGetProgramiv(shader.program_handle, GL_VALIDATE_STATUS, &validate_status);
        if (validate_status == GL_FALSE) {
            LM.write_log("Graphics_Manager::compile_shader(): Shader program is invalid in current OpenGL state.");
            return GL_FALSE;
        }
        LM.write_log("Graphics_Manager::compile_shader(): Shader program is validated and ready to execute in current OpenGL state.");
        return GL_TRUE;

    }


    void Graphics_Manager::program_use(GLuint program_handle)
    {
        if (program_handle > 0) {
            glUseProgram(program_handle);
        }
    }

    // Free shader program
    void Graphics_Manager::program_free() { glUseProgram(0); }

    GLuint Graphics_Manager::get_shader_program_handle(Assets_Manager::ShaderProgram shader) const {
        return shader.program_handle;
    }

} // namespace lof