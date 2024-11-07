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

        // Read file to initialize shaders 
        std::string vertex_obj_path = Path_Helper::get_vertex_shader_obj_path();
        std::string fragment_obj_path = Path_Helper::get_fragment_shader_obj_path();
        std::string vertex_debug_path = Path_Helper::get_vertex_shader_debug_path();
        std::string fragment_debug_path = Path_Helper::get_fragment_shader_debug_path(); 
        std::string vertex_font_path = Path_Helper::get_vertex_shader_font_path();
        std::string fragment_font_path = Path_Helper::get_fragment_shader_font_path(); 

        std::vector<std::pair<std::string, std::string>> shader_files{ // vertex & fragment shader files
        std::make_pair<std::string, std::string>(vertex_obj_path.c_str(), fragment_obj_path.c_str()),
        std::make_pair<std::string, std::string>(vertex_debug_path.c_str(), fragment_debug_path.c_str()),
        std::make_pair<std::string, std::string>(vertex_font_path.c_str(), fragment_font_path.c_str())
        };

        // Create shader program from shader files and insert 
        // shader program into shader container 
        if (!add_shader_program(shader_files)) {
            LM.write_log("Graphics_Manager::start_up(): Fail to add shader program.");
            return -1;
        } else {
            LM.write_log("Graphics_Manager::start_up(): Succesfully added shader programs.");
        }

        std::string mesh_path = Path_Helper::get_model_file_path();
        std::string texture_path = Path_Helper::get_texture_file_path();
        std::string animation_path = Path_Helper::get_animation_file_path();
        std::string font_path = Path_Helper::get_font_file_path();

        // Add models
        if (!add_model(mesh_path.c_str())) {
            LM.write_log("Fail to add models.");
            return -2;
        }

        // Add textures
        if (!add_textures(texture_path.c_str())) {
            LM.write_log("Fail to add textures.");
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
        shader_program_storage.clear();
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

    // Add a shader program into the shader program storage
    GLboolean Graphics_Manager::add_shader_program(std::vector<std::pair<std::string, std::string>> shaders) {
        for (auto const& file : shaders) {
            std::vector<std::pair<GLenum, std::string>> shader_files;
            shader_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, file.first));
            shader_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, file.second));

            // Compile the shaders to make a shader program
            ShaderProgram shader_program;
            if (!(Graphics_Manager::compile_shader(shader_files, shader_program))) {
                LM.write_log("Graphics_Manager::add_shader_program(): Shader program failed to compile.");
                return GL_FALSE;
            }
            // Insert shader program into container
            shader_program_storage.emplace_back(shader_program);
            std::size_t shader_idx = shader_program_storage.size() - 1;
            LM.write_log("Graphics_Manager::add_shader_program(): Shader program handle is %u.", shader_program.program_handle);
            LM.write_log("Graphics_Manager::add_shader_program(): Shader program %u created, compiled and added successfully.", shader_idx);
        }
        return GL_TRUE;
    }

    // Add models into the model storage
    GLboolean Graphics_Manager::add_model(std::string const& file_name) {

        // Read file containing model data
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            input_file.close();
            LM.write_log("Unable to open %s", file_name.c_str());
            return GL_FALSE;
        }
        input_file.seekg(0, std::ios::beg);

        // Model data
        struct TexVtxData { 
            glm::vec2 pos{}; 
            glm::vec2 tex{}; 
        };
        std::vector<TexVtxData> TexVtxArr;
        std::vector<glm::vec2> pos_vtx;
        std::vector<GLushort> vtx_idx;
        Graphics_Manager::Model mdl{};

        // For reading file to store model data
        std::string model_name, prefix, file_line;
        GLboolean is_model_exist{ GL_FALSE };
        while (getline(input_file, file_line)) {
            // First, skip line if it's related to existing model
            if (is_model_exist) {
                continue;
            }
            std::istringstream file_line_ss{ file_line };
            file_line_ss >> prefix;
            if (prefix == "m") { // Get model name
                file_line_ss >> model_name;
                if (model_storage.find(model_name) != model_storage.end()) { // Check if model exists in storage
                    is_model_exist = GL_TRUE;
                }
                else {
                    is_model_exist = GL_FALSE;
                }
            }
            else if (prefix == "v") { // Get vertex position
                if (model_name == "square") { 
                    TexVtxData data; 
                    file_line_ss >> data.pos.x; 
                    file_line_ss >> data.pos.y;  
                    file_line_ss >> data.tex.x; 
                    file_line_ss >> data.tex.y;  
                    TexVtxArr.emplace_back(data); 
                }
                else {
                    glm::vec2 pos{}; 
                    file_line_ss >> pos.x; 
                    file_line_ss >> pos.y; 
                    pos_vtx.emplace_back(pos); 
                } 
            }
            else if (prefix == "i") { // Get vertex index
                GLushort idx;
                while (file_line_ss >> idx) {
                    vtx_idx.emplace_back(idx);
                }
            }
            else if (prefix == "t" || prefix == "f" || prefix == "s" || prefix == "l") { // Get primitive type
                if (prefix == "t") {
                    mdl.primitive_type = GL_TRIANGLES;
                }
                else if (prefix == "f") {
                    mdl.primitive_type = GL_TRIANGLE_FAN;
                }
                else if (prefix == "s") {
                    mdl.primitive_type = GL_TRIANGLE_STRIP;
                }
                else if (prefix == "l") {
                    mdl.primitive_type = GL_LINES;
                }
            }
            else if (prefix == "e") { // Indicates end of model data, signal to make model

                // Add position vertex and texture coordinates data together
                std::size_t data_size{}; 
                if (model_name == "square") { 
                    data_size = sizeof(TexVtxData); 
                } else {
                    data_size = sizeof(glm::vec2); 
                }
                LM.write_log("Data_size values are: %u", data_size); 

                // Setting up VBO, VAO, and EBO
                GLuint vbo_hdl;
                GLuint vaoid;
                GLuint ebo_hdl;

                // encapsulate information about contents of VBO and VBO handle to VAO 
                glCreateVertexArrays(1, &vaoid);

                if (model_name == "square") {
                    // Generate a VAO handle to encapsulate the VBO(s) and
                    // state of the mesh
                    glCreateBuffers(1, &vbo_hdl);
                    glNamedBufferStorage(vbo_hdl, data_size * TexVtxArr.size(),
                        TexVtxArr.data(), GL_DYNAMIC_STORAGE_BIT);

                    // for assigning texture coordinates, we use vertex attribute index 1
                    // and vertex buffer binding point 7
                    glEnableVertexArrayAttrib(vaoid, 1);
                    glVertexArrayVertexBuffer(vaoid, 7, vbo_hdl, sizeof(glm::vec2), (GLsizei)data_size);
                    glVertexArrayAttribFormat(vaoid, 1, 2, GL_FLOAT, GL_FALSE, 0);
                    glVertexArrayAttribBinding(vaoid, 1, 7);

                }
                else {
                    // Generate a VAO handle to encapsulate the VBO(s) and
                    // state of the mesh
                    glCreateBuffers(1, &vbo_hdl);
                    glNamedBufferStorage(vbo_hdl, data_size * pos_vtx.size(),
                        pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
                }

                // for position vertex, we use vertex attribute index 0
                // and vertex buffer binding point 6 
                glEnableVertexArrayAttrib(vaoid, 0);
                glVertexArrayVertexBuffer(vaoid, 6, vbo_hdl, 0, (GLsizei)data_size);
                glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(vaoid, 0, 6);

                glCreateBuffers(1, &ebo_hdl);
                glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * vtx_idx.size(),
                    reinterpret_cast<GLvoid*>(vtx_idx.data()),
                    GL_DYNAMIC_STORAGE_BIT);
                glVertexArrayElementBuffer(vaoid, ebo_hdl);
                glBindVertexArray(0);

                // Return an appropriately initialized instance of a model
                mdl.vaoid = vaoid;
                mdl.draw_cnt = (GLuint)vtx_idx.size();
                model_storage[model_name] = mdl;
                LM.write_log("Graphics_Manager::add_model(): %s model successfully created and stored.", model_name.c_str());

                // Clear model data for next model
                pos_vtx.clear();
                vtx_idx.clear();
                model_name = "";
            }
        }
        // Close file
        input_file.close();
        LM.write_log("Graphics_Manager::add_model(): All models successfully created and stored.");
        return GL_TRUE;
    }

    // Add textures into the texture storage
    GLboolean Graphics_Manager::add_textures(std::string const& file_name) {
        // Create filepath for textures from texture name text file
        std::ifstream input_file{ file_name, std::ios::in }; 
        if (!input_file) { 
            input_file.close(); 
            LM.write_log("Unable to open %s", file_name.c_str()); 
            return GL_FALSE; 
        }
        input_file.seekg(0, std::ios::beg);

        std::string tex_name; 
        while (getline(input_file, tex_name)) {
            std::string tex_filepath = "../lack_of_oxygen/Data/Textures/" + tex_name + ".png";

            // Checking if file exists
            std::ifstream ifs{ tex_filepath, std::ios::binary };
            if (!ifs) {
                LM.write_log("Graphics_Manager::add_texture(): %s does not exist!!!!!", tex_filepath.c_str());
                return GL_FALSE; 
            }
            ifs.seekg(0, std::ios::beg);

            int width{ 0 }, height{ 0 }, channels{ 0 };
            stbi_set_flip_vertically_on_load(1);
            unsigned char* tex_data = stbi_load(tex_filepath.c_str(), &width, &height, &channels, 4);

            // Create and initialize a texture object that encapsulates a 2D texture
            GLuint tex_id{};
            if (tex_data) {
                glGenTextures(1, &tex_id);
                glBindTexture(GL_TEXTURE_2D, tex_id);

                // Set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                // Allocate memory for texture data and upload it
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
                glBindTexture(GL_TEXTURE_2D, 0);

                // Free data
                stbi_image_free(tex_data);
            }
            else {
                stbi_image_free(tex_data);
                LM.write_log("Graphics_Manager::add_texture(): %s texture data failed to load.", tex_name.c_str());
                return GL_FALSE;
            }

            // Add texture object id to texture storage
            texture_storage[tex_name] = tex_id;
            //LM.write_log("Graphics_Manager::add_texture(): tex_id = %u.", tex_id);
            LM.write_log("Graphics_Manager::add_texture(): %s texture data successfully added.", tex_name.c_str());
        }
        // Close file
        input_file.close();
        LM.write_log("Graphics_Manager::add_texture(): All textures successfully created and stored.");
        return GL_TRUE;
    }

    // Add animations into the animation storage
    GLboolean Graphics_Manager::add_animations(std::string const& file_name) {
        // Create filepath for animations from texture atlas text file
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            input_file.close();
            LM.write_log("Unable to open %s", file_name.c_str());
            return GL_FALSE;
        }
        input_file.seekg(0, std::ios::beg);

        // Variables to extract data to store into an animation
        std::string file_line, prefix;
        std::string a_name, t_name;
        Animation animation{};
        Frame frame{};

        while (getline(input_file, file_line)) {
            // Reading the animation data line by line
            std::istringstream file_line_ss{ file_line };
            file_line_ss >> prefix;
            if (prefix == "name") { // Get animation name
                file_line_ss >> a_name;
            }
            else if (prefix == "texture") {   // Get texture name
                file_line_ss >> animation.texture_name;
            }
            else if (prefix == "tex_width") {
                file_line_ss >> animation.tex_w;
            }
            else if (prefix == "tex_height") {
                file_line_ss >> animation.tex_h;
            }
            else if (prefix == "pos") {
                file_line_ss >> frame.uv_x;

                // Edit Y value to read from bottom left instead of top left for OpenGL texture reading
                float temp_y;
                file_line_ss >> temp_y;
                frame.uv_y = animation.tex_h - temp_y - DEFAULT_Y_OFFSET;
            }
            else if (prefix == "size") {
                file_line_ss >> frame.size;
            }
            else if (prefix == "time_delay") {
                file_line_ss >> frame.time_delay;
            }
            else if (prefix == "EF") { // End of frame data
                animation.frames.emplace_back(frame);
            }
            else if (prefix == "EA") { // End of animation data
                animation.frame_elapsed_time = DEFAULT_FRAME_TIME_ELAPSED; 
                animation_storage[a_name] = animation;
                animation = {}; // Clear animation data
                LM.write_log("Graphics_Manager::add_animations(): %s animation successfully created and stored.", a_name.c_str());
            }
        }

        // Close file
        input_file.close();
        LM.write_log("Graphics_Manager::add_animations(): All animations successfully created and stored.");
        return GL_TRUE;
    }

    // Add fonts into the font storage
    GLboolean Graphics_Manager::add_fonts(std::string const& file_name) {
        // Create filepath to access the fonts in fonts.txt file
        std::ifstream input_file{ file_name, std::ios::in }; 
        if (!input_file) { 
            input_file.close(); 
            LM.write_log("Unable to open %s", file_name.c_str()); 
            return GL_FALSE; 
        }
        input_file.seekg(0, std::ios::beg); 

        // Variables to extract data to store into an animation 
        std::string font_name; 
    
        while (getline(input_file, font_name)) {
            FT_Library font_type {};
            if (FT_Init_FreeType(&font_type)) {
                LM.write_log("Graphics_Manager::add_fonts(): Could not initialize FreeType Library");
                return GL_FALSE; 
            }
            
            // Checking if file exists
            std::string font_filepath = "../lack_of_oxygen/Data/Fonts/" + font_name + ".ttf";
            std::ifstream ifs{ font_filepath, std::ios::binary };
            if (!ifs) {
                LM.write_log("Graphics_Manager::add_fonts(): %s does not exist!!!!!", font_filepath.c_str());
                return GL_FALSE; 
            }
            ifs.seekg(0, std::ios::beg); 

            // Load font as face
            FT_Face face;
            if (FT_New_Face(font_type, font_filepath.c_str(), 0, &face)) {
                LM.write_log("Graphics_Manager::add_fonts(): Failed to load font %s", font_name.c_str()); 
                return GL_FALSE; 
            } else {
                // Set glyph size to load
                FT_Set_Pixel_Sizes(face, DEFAULT_GLYPH_WIDTH, DEFAULT_GLYPH_HEIGHT); 

                // Disable byte-alignment restriction
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                // Load first 128 characters of ASCII set
                std::map<GLchar, Character> characters_set; 
                Font new_font {}; 
                for (unsigned char ch = 0; ch < 128; ++ch) { 
                    // Load character glyph 
                    if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
                        LM.write_log("Graphics_Manager::add_fonts(): Failed to load Glyph");
                        return GL_FALSE; 
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

                    // Store characters into the characters container
                    Character character = {
                        texture,
                        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                        static_cast<unsigned int>(face->glyph->advance.x)
                    };
                    characters_set.insert(std::pair<char, Character>(ch, character));
                }

                // Encapsulate information about contents of VBO and VBO handle to VAO
                // to create a texture quad
                GLuint vbo_hdl, vaoid;
                glGenVertexArrays(1, &vaoid);           // Generate vao
                glGenBuffers(1, &vbo_hdl);              // Generate vbo
                glBindVertexArray(vaoid);               // Bind vao
                glBindBuffer(GL_ARRAY_BUFFER, vbo_hdl); // Bind vbo
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // Allocate memory in vbo
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);       // Unbind vbo
                glBindVertexArray(0);                   // Unbind vao

                // Store data such as handle to vao and characters
                new_font.vaoid = vaoid; 
                new_font.vboid = vbo_hdl;
                new_font.characters = characters_set;

                // Store the new font into font storage
                font_storage[font_name] = new_font;
                LM.write_log("Graphics_Manager::add_fonts(): Font %s successfully added.", font_name.c_str());
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            // Free FreeType
            FT_Done_Face(face);
            FT_Done_FreeType(font_type); 
        }
        // Close file
        input_file.close(); 
        LM.write_log("Graphics_Manager::add_fonts(): All fonts successfully created and stored."); 
        return GL_TRUE;
    }

    // Return reference to shader program storage
    Graphics_Manager::SHADERS& Graphics_Manager::get_shader_program_storage() { return shader_program_storage; }

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

    // Compilation of vertex and fragment shaders to make a shader program
    GLboolean Graphics_Manager::compile_shader(std::vector<std::pair<GLenum, std::string>> shader_files, ShaderProgram& shader) {
        // Read each shader file details such as shader type and file path
        for (auto& file : shader_files) {
            // Check if file's state is good for reading
            std::string const file_name = file.second.c_str();
            std::ifstream input_file(file_name);
            if (input_file.good() == GL_FALSE) {
                LM.write_log("Graphics_Manager::compile_shader(): File %s has error.", file_name.c_str());
                return GL_FALSE;
            }
            LM.write_log("Graphics_Manager::compile_shader(): File %s is good for reading.", file_name.c_str());

            // Create shader program
            if (shader.program_handle <= 0) {
                shader.program_handle = glCreateProgram();
                if (shader.program_handle == 0) {
                    LM.write_log("Graphics_Manager::compile_shader(): Cannot create program handle");
                    return GL_FALSE;
                }
                LM.write_log("Graphics_Manager::compile_shader(): Program handle %u created", shader.program_handle);
            }

            // Read code from shader file
            std::ifstream istream_file(file_name, std::ifstream::in);
            if (!istream_file.is_open()) { 
                LM.write_log("Graphics_Manager::compile_shader(): Error opening file %s.", file_name.c_str());
                return GL_FALSE; 
            }

            std::stringstream ss; 
            ss << istream_file.rdbuf(); 
            istream_file.close(); 
            std::string const shader_src = ss.str();
            GLchar const* shader_code[] = { shader_src.c_str()};

            // Create shader object and load shader code with it
            GLuint shader_obj = 0;
            if (file.first == GL_VERTEX_SHADER) {
                shader_obj = glCreateShader(GL_VERTEX_SHADER);
            } else if (file.first == GL_FRAGMENT_SHADER) {
                shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
            } else {
                LM.write_log("Graphics_Manager::compile_shader(): Invalid shader type.");
                return GL_FALSE;
            }
            glShaderSource(shader_obj, 1, shader_code, NULL);

            // Compile and check if successful
            glCompileShader(shader_obj);
            GLint compile_status;
            glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &compile_status);
            if (compile_status == GL_FALSE) {
                LM.write_log("Graphics_Manager::compile_shader(): Shader from file %s compilation fail.", file_name.c_str());
                return GL_FALSE; 
            } else {
                glAttachShader(shader.program_handle, shader_obj); 
                LM.write_log("Graphics_Manager::compile_shader(): Shader from file %s compiled successfully.", file_name.c_str());
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

    // Get a shader program ready to use
    void Graphics_Manager::program_use(ShaderProgram shader) {
        if (shader.program_handle > 0 && shader.link_status == GL_TRUE) {
            glUseProgram(shader.program_handle);
        }
    }

    // Free shader program
    void Graphics_Manager::program_free() { glUseProgram(0); }

    // Return the handle of the shader program
    GLuint Graphics_Manager::get_shader_program_handle(ShaderProgram shader) const {
        return shader.program_handle;
    }

} // namespace lof