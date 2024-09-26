/**
 * @file Render_System.cpp
 * @brief Implements the Render_System class for the ECS.
 * @author Chua Wen Bin Kenny
 * @date September 18, 2024
 */

 // Include header file
#include "Render_System.h"

namespace lof {

    Render_System::Render_System(ECS_Manager& ecs_manager) : ecs_manager(ecs_manager) {}

    std::string Render_System::get_type() const {
        return "Render_System";
    }

    // Updates the model-to-world-to-NDC transformation matrix of the component per frame.
    void Render_System::update(float delta_time) {
        for (const auto& entity_ptr : ecs_manager.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Check if the entity has Graphics and Transform components 
            if (entity_ptr->has_component(ecs_manager.get_component_id<Graphics_Component>())) {

                auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
                auto& transform = ecs_manager.get_component<Transform2D>(entity_id); 
                //auto& velocity= ecs_manager.get_component<Velocity_Component>(entity_id);

                // Compute object scale matrix
                glm::mat3 scale_mat{ transform.scale.x, 0, 0,
                                     0, transform.scale.y, 0,
                                     0, 0, 1 };

                //// Compute current orientation of object (Uncomment to rotate)
                //GLfloat angle_speed = 30.0f; // FOR TESTING (HARDCODED VALUE) 
                //transform.rotation += (angle_speed * (GLfloat)delta_time); 
                GLfloat rad_disp = glm::radians(transform.rotation); 

                // Compute object rotational matrix 
                glm::mat3 rot_mat{ glm::cos(rad_disp), glm::sin(rad_disp), 0,
                                  -glm::sin(rad_disp), glm::cos(rad_disp), 0,
                                  0, 0, 1 };

                // Compute object translation matrix
                glm::mat3 trans_mat{ 1, 0, 0,
                                     0, 1, 0,
                                     transform.position.x, transform.position.y, 1 };

                // Compute world scale matrix
                double const WorldRange{ 3000.0 };
                glm::mat3 world_scale{ 1.0 / WorldRange, 0, 0,
                                       0, 1.0 / WorldRange, 0,
                                       0, 0, 1 };

                // Compute model-to-world-to-NDC transformation matrix and store it in the component
                graphics.mdl_to_ndc_xform = world_scale * trans_mat * rot_mat * scale_mat;
                LM.write_log("Render_System::update(): entity ID %u update successful. Updated position is %f, %f.", entity_id, transform.position.x, transform.position.y);
            }
        }

        // Set up for the drawing of objects
        glClear(GL_COLOR_BUFFER_BIT);

        // Render polygon according to rendering mode 
        glPolygonMode(GL_FRONT_AND_BACK, GFXM.get_render_mode());
        switch (GFXM.get_render_mode()) {
        case GL_LINE:
            glLineWidth(5.0f);
            break;
        case GL_POINT:
            glPointSize(10.0f);
            break;
        default:
            break;
        }
        
        // Draw objects
        Render_System::draw();
    }

    // Renders the entity onto the window based on the graphics components
    void Render_System::draw() {
        
         
        // Provide model-to-world-to-NDC transformation matrix to vertex shader
        // Render and cleaning up
        for (const auto& entity_ptr : ecs_manager.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Check if the entity has Graphics and Transform components
            if (entity_ptr->has_component(ecs_manager.get_component_id<Graphics_Component>()) &&
                entity_ptr->has_component(ecs_manager.get_component_id<Transform2D>())) {

                auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
                auto& transform = ecs_manager.get_component<Transform2D>(entity_id);
                
                // Get shaders and models container 
                auto& shaders = GFXM.get_shader_program_storage();
                auto& models = GFXM.get_model_storage();

                // Start the shader program that the entity will use for rendering
                GFXM.program_use(shaders[graphics.shd_ref]);

                // Bind object's VAO handle
                glBindVertexArray(models[graphics.mdl_ref].vaoid);

                // Pass object's mdl_to_ndc_xform to vertex shader
                GLint uniform_location = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "uModel_to_NDC");
                if (uniform_location >= 0) {
                    glUniformMatrix3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(graphics.mdl_to_ndc_xform));
                } else {
                    LM.write_log("Render_System::draw(): Uniform variable doesn't exist!");
                    std::exit(EXIT_FAILURE);
                }

                // Render objects
                glDrawElements(models[graphics.mdl_ref].primitive_type, models[graphics.mdl_ref].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                LM.write_log("Render_System::draw(): entity ID %u rendered successfully at position %f, %f.", entity_id, transform.position.x, transform.position.y);

                // Clean up by unbinding the VAO and ending the shader program
                glBindVertexArray(0);
                GFXM.program_free();




                ///////////////////////////FOR TESTING////////////////////////////////////
                //auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id); 
                //auto& shaders = GFXM.get_shader_program_storage();
                //auto& models = GFXM.get_model_storage();
                //auto& transform = ecs_manager.get_component<Transform2D>(entity_id); // FOR DEBUGGING 
                //auto& model_comp = ecs_manager.get_component<Model_Component>(entity_id);
                // 
                // 
                //std::vector<Vec2D> pos_vtx{};
                //Vec2D test = { 1.0f, 2.0f };
                //pos_vtx.emplace_back(test);
                //std::vector<GLushort> vtx_idx; 
                //Graphics_Manager::Model mdl{};
                //mdl.primitive_type = GL_TRIANGLES; // Standardize it for testing
                //std::string name = model_comp.model_name;
                //LM.write_log("Render_System::draw(): Model name is %s.", name.c_str());
                //LM.write_log("Render_System::draw(): pos_vtx first value before is %f, %f.", (*pos_vtx.begin()).x, (*pos_vtx.begin()).y);
                //pos_vtx = (model_comp.model_ptr)->get_vertices();
                //LM.write_log("Render_System::draw(): pos_vtx first value after is %u, %u.", (*pos_vtx.begin()).x, (*pos_vtx.begin()).y);
                //LM.write_log("Render_System::draw(): Successfully get vertices.");
                //auto triangles_start = (*(model_comp.model_ptr)).get_triangles().begin();
                //auto triangles_end = (*(model_comp.model_ptr)).get_triangles().end();
                //while (triangles_start != triangles_end) {
                //    auto vertices_start = (*triangles_start).begin();
                //    auto vertices_end = (*triangles_start).end(); 
                //    while (vertices_start != vertices_end) {
                //        vtx_idx.emplace_back(static_cast<GLushort> (*vertices_start));
                //        vertices_start++; 
                //    }
                //    triangles_start++;
                //    
                //}

                //if (models.size() < 3) { // Add square mesh if <2, triangle mesh if <3
                //    // Generate a VAO handle to encapsulate the VBO(s) and
                //    // state of the triangle mesh
                //    GLuint vbo_hdl; 
                //    glCreateBuffers(1, &vbo_hdl); 
                //    glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), 
                //        pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT); 

                //    // encapsulate information about contents of VBO and VBO handle to VAO
                //    GLuint vaoID; 
                //    glCreateVertexArrays(1, &vaoID); 

                //    // for position vertex, we use vertex attribute index 0
                //    // and vertex buffer binding point 1
                //    glEnableVertexArrayAttrib(vaoID, 0); 
                //    glVertexArrayVertexBuffer(vaoID, 1, vbo_hdl, 0, sizeof(glm::vec2)); 
                //    glVertexArrayAttribFormat(vaoID, 0, 2, GL_FLOAT, GL_FALSE, 0); 
                //    glVertexArrayAttribBinding(vaoID, 0, 1); 
                //      
                //    GLuint ebo_hdl; 
                //    glCreateBuffers(1, &ebo_hdl); 
                //    glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * vtx_idx.size(), 
                //        reinterpret_cast<GLvoid*>(vtx_idx.data()), 
                //        GL_DYNAMIC_STORAGE_BIT);
                //    glVertexArrayElementBuffer(vaoID, ebo_hdl);
                //    glBindVertexArray(0);

                //    // Return an appropriately initialized instance of GLApp::GLModel
                //    mdl.vaoid = vaoID;
                //    mdl.primitive_cnt = pos_vtx.size() - 2; // number of primitives
                //    mdl.primitive_type = GL_TRIANGLES; 
                //    mdl.draw_cnt = vtx_idx.size(); // number of vertices
                //    std::size_t currSize = models.size();
                //    models[currSize] = mdl;
                //    graphics.mdl_ref = static_cast<GLuint> (currSize); 
                //} 

                //// Part 1: Install the shader program used by this object to
                //// render its model using GLSLShader::Use()
                //shaders[graphics.shd_ref].Use();

                //// Part 2: Bind object's VAO handle using glBindVertexArray
                //glBindVertexArray(models[graphics.mdl_ref].vaoid);

                //// Part 3: Copy object's 3x3 model-to-NDC matrix to vertex shader
                //GLint uniform_var_loc1 = glGetUniformLocation(
                //    shaders[graphics.shd_ref].GetHandle(), "uModel_to_NDC");
                //if (uniform_var_loc1 >= 0) {
                //    glUniformMatrix3fv(uniform_var_loc1, 1, GL_FALSE,
                //        glm::value_ptr(graphics.mdl_to_ndc_xform));
                //}
                //else {
                //    std::cout << "Uniform variable doesn't exist!!!\n";
                //    LM.write_log("Render_System::draw(): Uniform variable doesn't exist!!!");
                //    std::exit(EXIT_FAILURE);
                //}

                //// Part 4: Render using glDrawElements or glDrawArrays
                ////glDrawArrays(models[mdl_ref].primitive_type, 0, models[mdl_ref].draw_cnt);
                //glDrawElements(models[graphics.mdl_ref].primitive_type, models[graphics.mdl_ref].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                //LM.write_log("Render_System::draw(): entity ID %u rendered successfully at position %u, %u.", entity_id, transform.position.x, transform.position.y);

                //// Part 5: Clean up
                //// Breaking the binding set up in Part 2: glBindVertexArray(0);
                //// Deinstall the shader program installed in Part 1 using
                //// GLSLShader::UnUse()
                //glBindVertexArray(0);
                //shaders[graphics.shd_ref].UnUse();
                ///////////////////////////FOR TESTING////////////////////////////////////
            }
        }
    }

} // namespace lof
