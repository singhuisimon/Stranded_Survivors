/**
 * @file Render_System.cpp
 * @brief Implements the Render_System class for the ECS that
 *        updates the transformation and rendering details of the entity.
 * @author Chua Wen Bin Kenny (100%)
 * @date September 18, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Render_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"

namespace lof {

    Render_System::Render_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Graphics_Component>());
        signature.set(ECSM.get_component_id<Transform2D>());
    }

    std::string Render_System::get_type() const {
        return "Render_System";
    }

    // Updates the model-to-world-to-NDC transformation matrix of the entity per frame.
    void Render_System::update(float delta_time) {

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);

            if (entity_id != 0) { // Background object unaffected

                // Scaling update when up or down arrow key pressed
                GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);

                // Check if the entity has Collision_Component
                if (ECSM.has_component<Collision_Component>(entity_id)) {
                    auto& collision = ECSM.get_component<Collision_Component>(entity_id);

                    if (IM.is_key_held(GLFW_KEY_UP)) {
                        LM.write_log("Render_System::update(): 'UP' key held, increasing scale of entity %u by %f.", entity_id, scale_change);
                        transform.scale.x += scale_change;
                        transform.scale.y += scale_change;
                        collision.width += scale_change;
                        collision.height += scale_change;
                    }
                    else if (IM.is_key_held(GLFW_KEY_DOWN)) {
                        LM.write_log("Render_System::update(): 'DOWN' key held, decreasing scale of entity %u by %f.", entity_id, scale_change);
                        if (transform.scale.x > 0.0f) {
                            transform.scale.x -= scale_change;
                            collision.width -= scale_change;
                        }
                        else {
                            transform.scale.x = 0.0f;
                            collision.width = 0.0f;
                        }

                        if (transform.scale.y > 0.0f) {
                            transform.scale.y -= scale_change;
                            collision.height -= scale_change;
                        }
                        else {
                            transform.scale.y = 0.0f;
                            collision.height = 0.0f;
                        }
                    }
                }

                // Rotation update when left or right arrow key pressed
                if (IM.is_key_held(GLFW_KEY_LEFT)) {
                    GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                    transform.orientation.x += rot_change;
                    LM.write_log("Render_System::update(): 'LEFT' key held, rotating entity %u by %f.", entity_id, rot_change);
                }
                else if (IM.is_key_held(GLFW_KEY_RIGHT)) {
                    GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                    transform.orientation.x -= rot_change;
                    LM.write_log("Render_System::update(): 'RIGHT' key held, rotating entity %u by %f.", entity_id, rot_change);
                }
            }

            // Compute object scale matrix
            glm::mat3 scale_mat{ transform.scale.x, 0, 0,
                                 0, transform.scale.y, 0,
                                 0, 0, 1 };

            // Compute current orientation of object
            GLfloat rad_disp = glm::radians(transform.orientation.x);

            // Compute object rotational matrix 
            glm::mat3 rot_mat{ glm::cos(rad_disp),  glm::sin(rad_disp), 0,
                               -glm::sin(rad_disp),  glm::cos(rad_disp), 0,
                                0,                   0,                  1 };

            // Compute object translation matrix
            glm::mat3 trans_mat{ 1, 0, 0,
                                 0, 1, 0,
                                 transform.position.x, transform.position.y, 1 };

            // Compute world scale matrix
            glm::mat3 world_scale{ 1.0f / SM.get_scr_width(), 0, 0,
                                   0, 1.0f / SM.get_scr_height(), 0,
                                   0, 0, 1 };

            // Compute model-to-world-to-NDC transformation matrix and store it in the graphics component
            graphics.mdl_to_ndc_xform = world_scale * trans_mat * rot_mat * scale_mat;
        }

        // Set up for the drawing of objects
        glClear(GL_COLOR_BUFFER_BIT);

        // Render polygon according to rendering mode 
        glPolygonMode(GL_FRONT_AND_BACK, GFXM.get_render_mode());
        switch (GFXM.get_render_mode()) {
        case GL_LINE:
            glLineWidth(DEFAULT_LINE_WIDTH);
            break;
        case GL_POINT:
            glPointSize(DEFAULT_POINT_SIZE);
            break;
        default:
            break;
        }

        // Enabling Alpha Blending to blend texture to background
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw all objects
        draw();
    }

    // Renders the entity onto the window based on the graphics components
    void Render_System::draw() {

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);

            // Get shaders, models, and textures from the Graphics Manager
            auto& shaders = GFXM.get_shader_program_storage();
            auto& models = GFXM.get_model_storage();
            auto& textures = GFXM.get_texture_storage();

            // Start the shader program that the entity will use for rendering 
            GFXM.program_use(shaders[graphics.shd_ref]);

            // Bind object's VAO handle
            glBindVertexArray(models[graphics.model_name].vaoid);

            // Check if entity has a texture
            if (graphics.texture_name != DEFAULT_TEXTURE_NAME) {
                // Assign texture object to use texture image unit 6 
                glBindTextureUnit(6, textures[graphics.texture_name]);
                LM.write_log("Render_System::draw(): Texture name: %s.", graphics.texture_name.c_str());

                // Set texture flag to true
                GLuint tex_flag_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "TexFlag");
                if (tex_flag_loc >= 0) {
                    glUniform1ui(tex_flag_loc, GL_TRUE);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture flag uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // Set texture unit in shader
                GLuint tex_uniform_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "uTex2d");
                if (tex_uniform_loc >= 0) {
                    glUniform1i(tex_uniform_loc, 6);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }
            }
            else {
                // Set texture flag to false
                GLuint tex_flag_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "TexFlag");
                if (tex_flag_loc >= 0) {
                    glUniform1ui(tex_flag_loc, GL_FALSE);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture flag uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }
            }

            // Pass object's color to fragment shader uniform variable uColor
            GLint color_uniform_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "uColor");
            if (color_uniform_loc >= 0) {
                glUniform3fv(color_uniform_loc, 1, &graphics.color[0]);
            }
            else {
                LM.write_log("Render_System::draw(): Color uniform variable doesn't exist.");
                std::exit(EXIT_FAILURE);
            }

            // Pass object's mdl_to_ndc_xform to vertex shader to compute object's final position
            GLint mat_uniform_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref]), "uModel_to_NDC_Mat");
            if (mat_uniform_loc >= 0) {
                glUniformMatrix3fv(mat_uniform_loc, 1, GL_FALSE, &graphics.mdl_to_ndc_xform[0][0]);
            }
            else {
                LM.write_log("Render_System::draw(): Matrix uniform variable doesn't exist.");
                std::exit(EXIT_FAILURE);
            }

            // Render objects
            if (entity_id == 0) { // Set background object to always render in fill mode
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(models[graphics.model_name].primitive_type, models[graphics.model_name].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                glPolygonMode(GL_FRONT_AND_BACK, GFXM.get_render_mode());
            }
            else {
                glDrawElements(models[graphics.model_name].primitive_type, models[graphics.model_name].draw_cnt, GL_UNSIGNED_SHORT, NULL);
            }

            // Draw debugging features if debug mode is ON
            if (entity_id != 0) { // Background object unaffected 
                if (GFXM.get_debug_mode() == GL_TRUE) {
                    // Check if entity has Velocity_Component and Collision_Component
                    bool has_velocity = ECSM.has_component<Velocity_Component>(entity_id);
                    bool has_collision = ECSM.has_component<Collision_Component>(entity_id);

                    if (has_velocity || has_collision) {

                        // Variables for debugging
                        auto& shaders = GFXM.get_shader_program_storage();
                        auto& models = GFXM.get_model_storage();

                        // Free texture shader program to allow models shader program to bind and start  
                        GFXM.program_free();
                        GFXM.program_use(shaders[graphics.shd_ref + 1]);

                        // Set draw color for debug shapes to black and pass to fragment shader uniform variable uColor
                        GLint debug_color_uniform_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref + 1]), "uColor");
                        glm::vec3 debug_color{ 0.0f, 0.0f, 0.0f };
                        if (debug_color_uniform_loc >= 0) {
                            glUniform3fv(debug_color_uniform_loc, 1, &debug_color[0]);
                        }
                        else {
                            LM.write_log("Render_System::draw(): Debug color uniform variable doesn't exist.");
                            std::exit(EXIT_FAILURE);
                        }

                        // Pass debug object's mdl_to_ndc_xform to vertex shader to compute final position
                        GLint debug_mat_uniform_loc = glGetUniformLocation(GFXM.get_shader_program_handle(shaders[graphics.shd_ref + 1]), "uModel_to_NDC_Mat");
                        if (debug_mat_uniform_loc < 0) {
                            LM.write_log("Render_System::draw(): Debug matrix uniform variable doesn't exist.");
                            std::exit(EXIT_FAILURE);
                        }

                        // Drawing collision box if entity has Collision_Component
                        if (has_collision) {
                            auto& collision = ECSM.get_component<Collision_Component>(entity_id);

                            std::vector<glm::mat3> box_mdl_to_ndc_xform;

                            // Compute mdl_to_ndc_xform for each line of AABB box
                            for (std::size_t i = 0; i < 4; ++i) {
                                GLfloat scale_width = collision.width;
                                GLfloat scale_height = collision.height;
                                if (i == 1 || i == 3) {
                                    scale_width = collision.height;
                                    scale_height = collision.width;
                                }

                                // Compute line's scale matrix
                                glm::mat3 AABB_scale_mat{ scale_width, 0, 0,
                                                          0, scale_height, 0,
                                                          0, 0, 1 };

                                // Compute current orientation of each line 
                                GLfloat AABB_rad_disp = glm::radians(i * DEFAULT_ROTATION);

                                // Compute line rotational matrix 
                                glm::mat3 AABB_rot_mat{ glm::cos(AABB_rad_disp),  glm::sin(AABB_rad_disp), 0,
                                                        -glm::sin(AABB_rad_disp),  glm::cos(AABB_rad_disp), 0,
                                                         0,                   0,                  1 };

                                // Compute line translation matrix
                                glm::mat3 AABB_trans_mat{ 1, 0, 0,
                                                          0, 1, 0,
                                                          transform.position.x, transform.position.y, 1 };

                                // Compute world scale matrix
                                glm::mat3 world_scale{ 1.0f / SM.get_scr_width(), 0, 0,
                                                       0, 1.0f / SM.get_scr_height(), 0,
                                                       0, 0, 1 };

                                // Compute model-to-world-to-NDC transformation matrix for line and store it
                                glm::mat3 result_xform = world_scale * AABB_trans_mat * AABB_rot_mat * AABB_scale_mat;
                                box_mdl_to_ndc_xform.emplace_back(result_xform);
                            }

                            // Drawing AABB box line by line
                            for (std::size_t i = 0; i < 4; ++i) {
                                glLineWidth(DEFAULT_AABB_WIDTH);
                                glUniformMatrix3fv(debug_mat_uniform_loc, 1, GL_FALSE, &box_mdl_to_ndc_xform[i][0][0]);
                                glBindVertexArray(models["debug_line"].vaoid);
                                glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                                glBindVertexArray(0);
                            }
                        }

                        // Drawing velocity vector if entity has Velocity_Component
                        if (has_velocity) {
                            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);

                            // Compute line scale matrix
                            glm::mat3 scale_mat{ transform.scale.x * DEFAULT_VELOCITY_LINE_LENGTH, 0, 0,
                                                 0, transform.scale.y * DEFAULT_VELOCITY_LINE_LENGTH, 0,
                                                 0, 0, 1 };

                            // Compute direction based on velocity
                            GLfloat direction = glm::degrees(glm::atan(velocity.velocity.y, velocity.velocity.x));

                            // Convert direction to radians
                            GLfloat rad_disp = glm::radians(direction);

                            // Compute line rotational matrix 
                            glm::mat3 rot_mat{ glm::cos(rad_disp),  glm::sin(rad_disp), 0,
                                               -glm::sin(rad_disp),  glm::cos(rad_disp), 0,
                                                0,                   0,                  1 };

                            // Compute line translation matrix
                            glm::mat3 trans_mat{ 1, 0, 0,
                                                 0, 1, 0,
                                                 transform.position.x, transform.position.y, 1 };

                            // Compute world scale matrix
                            glm::mat3 world_scale{ 1.0f / SM.get_scr_width(), 0, 0,
                                                   0, 1.0f / SM.get_scr_height(), 0,
                                                   0, 0, 1 };

                            // Compute model-to-world-to-NDC transformation matrix for the velocity line
                            glm::mat3 line_mdl_to_ndc_xform = world_scale * trans_mat * rot_mat * scale_mat;

                            glUniformMatrix3fv(debug_mat_uniform_loc, 1, GL_FALSE, &line_mdl_to_ndc_xform[0][0]);
                            glLineWidth(DEFAULT_LINE_WIDTH);
                            glBindVertexArray(models["debug_line"].vaoid);
                            glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                            glBindVertexArray(0);
                        }
                    }
                }

                // Clean up by unbinding the VAO and ending the shader program
                glBindVertexArray(0);
                GFXM.program_free();
            }
        }
    }

} // namespace lof
