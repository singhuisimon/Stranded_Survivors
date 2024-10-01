/**
 * @file Graphics_Manager.cpp
 * @brief Implements the Graphics_Manager class methods.
 * @author Chua Wen Bin Kenny
 * @date September 18, 2024
 */

 // Include header file
#include "Graphics_Manager.h" 

namespace lof {

    std::unique_ptr<Graphics_Manager> Graphics_Manager::instance;
    std::once_flag Graphics_Manager::once_flag;


    Graphics_Manager::Graphics_Manager() {
        set_type("Graphics_Manager");
        m_is_started = false;
        render_mode = GL_FILL;
        set_time(0);
    }


    Graphics_Manager::~Graphics_Manager() {
        if (is_started()) {
            shut_down();
        }
    }


    Graphics_Manager& Graphics_Manager::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Graphics_Manager);
            });
        return *instance;
    }


    int Graphics_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // Set framebuffer with color (Background color)
        glClearColor(1.f, 1.f, 1.f, 1.f);

        // Set viewport position and dimensions
        glViewport(0, 0, 1024, 768);

        // Set up default render mode 
        render_mode = GL_FILL; 

        // Read file to initialize shaders 
        std::vector<std::pair<std::string, std::string>> shader_files{ // vertex & fragment shader files
        std::make_pair<std::string, std::string> ("../lack_of_oxygen/Shaders/my-tutorial-3.vert", 
                                                  "../lack_of_oxygen/Shaders/my-tutorial-3.frag")
        };

        // Create shader program from shader files and insert 
        // shader program into shader container 
        if (!add_shader_program(shader_files)) {
            LM.write_log("Graphics_Manager::start_up(): Fail to add shader program.");
            return -1;
        } else {
            LM.write_log("Graphics_Manager::start_up(): Succesfully added shader program.");
        }

        // Add models (Will move this to serialization side)
        if (!add_model()) {
            LM.write_log("Fail to add model.");
            return -1;
        } else {
            LM.write_log("Graphics_Manager::start_up(): Succesfully added model.");
        }

        m_is_started = true;
        return 0;
    }

    void Graphics_Manager::shut_down() {
        if (!is_started()) {
            return; // Not started
        }

        // Free resources if needed

        m_is_started = false;
    }

    void Graphics_Manager::update() { // Update details, add mesh/textures/sprits/fonts, etc
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
    }

    // Add a shader program into the shader program storage.
    GLboolean Graphics_Manager::add_shader_program(std::vector<std::pair<std::string, std::string>> shaders) {
        for (auto const& file : shaders) {
            std::vector<std::pair<GLenum, std::string>> shader_files;
            shader_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, file.first));
            shader_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, file.second));

            ShaderProgram shader_program;
            if (!(Graphics_Manager::compile_shader(shader_files, shader_program))) {
                LM.write_log("Graphics_Manager::add_shader_program(): Shader program failed to compile.");
                return GL_FALSE;
            }
            // Insert shader program into container
            shader_program_storage.emplace_back(shader_program);
            LM.write_log("shader_program.program_handle value is %u", shader_program.program_handle);
            LM.write_log("Graphics_Manager::add_shader_program(): Shader program compiled successfully.");
        }

        return GL_TRUE;
    }

    // Add a model into the model storage.
    GLboolean Graphics_Manager::add_model() {

        ///////////////////////////// SQUARE ////////////////////////////////////////
        // store vertices in a box centered at (0,0) with size 1 x 1 defined in NDC
        GLushort const slice_size{ 2 };
        GLushort const stack_size{ 2 };
        std::vector<glm::vec2> pos_vtx(slice_size * stack_size);
        for (GLushort stack = 0, i = 0; stack < stack_size; ++stack) {
            for (GLushort slice = 0; slice < slice_size; ++slice) {
                pos_vtx[i].x = ((1.0f / (float)(slice_size - 1)) * (float)slice - 0.5f);
                pos_vtx[i].y = ((1.0f / (float)(stack_size - 1)) * (float)stack - 0.5f);
                i++;
            }
        }

        // randomly generate rgb values for each point and store
        // them in a seperate vector container
        std::vector<glm::vec3> clr_vtx(pos_vtx.size());
        for (size_t i = 0; i < pos_vtx.size(); ++i) {
            clr_vtx[i].x = 0.00f;
            clr_vtx[i].y = 0.00f;
            clr_vtx[i].z = 1.00f;
        }

        std::vector<GLushort> vtx_idx;
        for (GLushort row = 1; row < stack_size; ++row) {
            GLushort strip_vtx = slice_size * row;
            for (GLushort col = 0; col < slice_size; ++col) {
                vtx_idx.emplace_back((unsigned short)(strip_vtx + col));
                vtx_idx.emplace_back((unsigned short)(strip_vtx + col - slice_size));
            }
        }

        // Generate a VAO handle to encapsulate the VBO(s) and
        // state of the triangle mesh
        GLuint vbo_hdl;
        glCreateBuffers(1, &vbo_hdl);
        glNamedBufferStorage(vbo_hdl,
            sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
            nullptr, GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferSubData(vbo_hdl, 0,
            sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
        glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
            sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

        // encapsulate information about contents of VBO and VBO handle to VAO
        GLuint vaoid;
        glCreateVertexArrays(1, &vaoid);

        // for position vertex, we use vertex attribute index 0
        // and vertex buffer binding point 0
        glEnableVertexArrayAttrib(vaoid, 0);
        glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
        glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vaoid, 0, 0);

        // for vertex color array, we use vertex attribute index 1
        // and vertex buffer binding point 1
        glEnableVertexArrayAttrib(vaoid, 1);
        glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl,
            sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3));
        glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vaoid, 1, 1);

        GLuint ebo_hdl;
        glCreateBuffers(1, &ebo_hdl);
        glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * vtx_idx.size(),
            reinterpret_cast<GLvoid*>(vtx_idx.data()),
            GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(vaoid, ebo_hdl);
        glBindVertexArray(0);

        // Return an appropriately initialized instance of GLApp::GLModel
        Graphics_Manager::Model mdl;
        mdl.vaoid = vaoid;
        mdl.primitive_type = GL_TRIANGLE_STRIP;
        mdl.draw_cnt = (GLuint)vtx_idx.size();
        mdl.primitive_cnt = 2;
        model_storage.emplace_back(mdl);
        ///////////////////////////// SQUARE ////////////////////////////////////////

        ///////////////////////////// STAR ////////////////////////////////////////
        //// store vertices in a box centered at (0,0) with size 1 x 1 defined in NDC
        //GLushort n_tri = 4;
        //std::vector<glm::vec2>pos_vtx(n_tri * 2);

        //pos_vtx[0].x = 0.0f;
        //pos_vtx[0].y = 1.0f;
        //pos_vtx[1].x = -0.30f;
        //pos_vtx[1].y = 0.30f;
        //pos_vtx[2].x = -1.0f;
        //pos_vtx[2].y = 0.0f;
        //pos_vtx[3].x = -0.30f;
        //pos_vtx[3].y = -0.30f;

        //pos_vtx[4].x = 0.0f;
        //pos_vtx[4].y = -1.0f;
        //pos_vtx[5].x = 0.30f;
        //pos_vtx[5].y = -0.30f;
        //pos_vtx[6].x = 1.0f;
        //pos_vtx[6].y = 0.0f;
        //pos_vtx[7].x = 0.30f;
        //pos_vtx[7].y = 0.30f;


        //// randomly generate rgb values for each point and store
        //// them in a seperate vector container
        ////std::uniform_real_distribution<float> urdf(0.0f, 1.0f);
        //std::vector<glm::vec3> clr_vtx(pos_vtx.size());
        //for (size_t i = 0; i < pos_vtx.size(); ++i) {
        //    clr_vtx[i].x = 0.0f;
        //    clr_vtx[i].y = 0.0f;
        //    clr_vtx[i].z = 1.0f;
        //}


        //// index buffer for mystery model
        //std::vector<GLushort> vtx_idx{ 0, 7, 1, 3, 2, 1, 3, 5, 4, 3, 5, 7, 6 };

        //// Generate a VAO handle to encapsulate the VBO(s) and
        //// state of the triangle mesh
        //GLuint vbo_hdl;
        //glCreateBuffers(1, &vbo_hdl);

        //glNamedBufferStorage(vbo_hdl,
        //    sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
        //    nullptr, GL_DYNAMIC_STORAGE_BIT);
        //glNamedBufferSubData(vbo_hdl, 0,
        //    sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
        //glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
        //    sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

        //// encapsulate information about contents of VBO and VBO handle to VAO
        //GLuint vaoid;
        //glCreateVertexArrays(1, &vaoid);

        //// for position vertex, we use vertex attribute index 0
        //// and vertex buffer binding point 2
        //glEnableVertexArrayAttrib(vaoid, 0);
        //glVertexArrayVertexBuffer(vaoid, 2, vbo_hdl, 0, sizeof(glm::vec2));
        //glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
        //glVertexArrayAttribBinding(vaoid, 0, 2);

        //// for vertex color array, we use vertex attribute index 1
        //// and vertex buffer binding point 4
        //glEnableVertexArrayAttrib(vaoid, 1);
        //glVertexArrayVertexBuffer(vaoid, 4, vbo_hdl,
        //    sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3));
        //glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
        //glVertexArrayAttribBinding(vaoid, 1, 4);

        //GLuint ebo_hdl;
        //glCreateBuffers(1, &ebo_hdl);
        //glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * vtx_idx.size(),
        //    reinterpret_cast<GLvoid*>(vtx_idx.data()),
        //    GL_DYNAMIC_STORAGE_BIT);
        //glVertexArrayElementBuffer(vaoid, ebo_hdl);
        //glBindVertexArray(0);

        //// Return an appropriately initialized instance of GLApp::GLModel
        //Graphics_Manager::Model mdl;
        //mdl.vaoid = vaoid;
        //mdl.primitive_type = GL_TRIANGLE_STRIP;
        //mdl.draw_cnt = vtx_idx.size();
        //mdl.primitive_cnt = n_tri * 3 - 1;
        //model_storage.emplace_back(mdl); 
        ///////////////////////////// STAR ////////////////////////////////////////

        LM.write_log("Graphics_Manager::add_model(): Model successfully created.");
        return GL_TRUE;
    }

    Graphics_Manager::SHADERS& Graphics_Manager::get_shader_program_storage() {
        return shader_program_storage;
    }

    Graphics_Manager::MODELS& Graphics_Manager::get_model_storage() {
        return model_storage;
    }

    GLenum& Graphics_Manager::get_render_mode() {
        return render_mode;
    }

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
                LM.write_log("Graphics_Manager::compile_shader(): Program handle created");
            }

            // Read code from shader file
            std::ifstream istream_file(file_name, std::ifstream::in);
            if (!istream_file.is_open()) { 
                LM.write_log("Graphics_Manager::compile_shader(): Error opening file %s.", file_name.c_str());
                return GL_FALSE; 
            }
            //std::stringstream ss; 
            //ss << istream_file.rdbuf(); 
            //istream_file.close(); 
            //GLchar const* shader_code[] = { (ss.str()).c_str()};

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

    void Graphics_Manager::program_use(ShaderProgram shader) {
        if (shader.program_handle > 0 && shader.link_status == GL_TRUE) {
            glUseProgram(shader.program_handle);
        }
    }

    void Graphics_Manager::program_free() {
        glUseProgram(0);
    }

    GLuint Graphics_Manager::get_shader_program_handle(ShaderProgram shader) const {
        return shader.program_handle;
    }


} // namespace lof