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

namespace lof {

    Render_System::Render_System(ECS_Manager& ecs_manager) : ecs_manager(ecs_manager) {
        set_time(DEFAULT_START_TIME);
    }

    std::string Render_System::get_type() const {
        return "Render_System";
    }

    // Updates the model-to-world-to-NDC transformation matrix of the entity per frame.
    void Render_System::update(float delta_time) {

        for (const auto& entity_ptr : ecs_manager.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Check if the entity has Graphics, Transform and Collision components 
            if (entity_ptr->has_component(ecs_manager.get_component_id<Graphics_Component>()) && 
                entity_ptr->has_component(ecs_manager.get_component_id<Transform2D>())) { 

                auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
                auto& transform = ecs_manager.get_component<Transform2D>(entity_id);

                // Update component if debug mode is ON
                if (GFXM.get_debug_mode() == GL_TRUE) {
                    if (entity_id != 0) { // Temporary object background unaffected
                        // Scaling update when up or down arrow key pressed
                        auto& collision = ecs_manager.get_component<Collision_Component>(entity_id);
                        GLfloat scale_change = DEFAULT_SCALE_CHANGE * (GLfloat)delta_time; 
                        if (IM.is_key_held(GLFW_KEY_UP)) {
                            LM.write_log("Render_System::update(): 'UP' key pressed, scale of entity %u increased by %f.", entity_id, scale_change);
                            transform.scale.x += scale_change;
                            transform.scale.y += scale_change;
                            collision.width   += scale_change;
                            collision.height  += scale_change;
                        }
                        else if (IM.is_key_held(GLFW_KEY_DOWN)) {
                            LM.write_log("Render_System::update(): 'DOWN' key pressed, scale of entity %u decreased by %f.", entity_id, scale_change);
                            if (transform.scale.x > 0.0f) {
                                transform.scale.x -= scale_change;
                                collision.width   -= scale_change;  
                            }
                            else {
                                transform.scale.x = 0.0f;
                                collision.width   = 0.0f;
                            }

                            if (transform.scale.y > 0.0f) {
                                transform.scale.y -= scale_change;
                                collision.height  -= scale_change;
                            }
                            else {
                                transform.scale.y = 0.0f;
                                collision.height  = 0.0f;
                            }
                        }

                        // Rotation update when left or right arrow key pressed
                        if (IM.is_key_held(GLFW_KEY_LEFT)) {
                            GLfloat rot_change = transform.orientation.y * (GLfloat)delta_time;
                            transform.orientation.x += rot_change;
                            LM.write_log("Render_System::update(): 'LEFT' key pressed, entity %u rotated by %f.", entity_id, rot_change);
                        }
                        else if (IM.is_key_held(GLFW_KEY_RIGHT)) {
                            GLfloat rot_change = transform.orientation.y * (GLfloat)delta_time;
                            transform.orientation.x -= rot_change;
                            LM.write_log("Render_System::update(): 'RIGHT' key pressed, entity %u rotated by %f.", entity_id, rot_change);
                        }
                    }
                }

                // Compute object scale matrix
                glm::mat3 scale_mat{ transform.scale.x, 0, 0,
                                     0, transform.scale.y, 0,
                                     0, 0, 1 };

                // Compute current orientation of object (Uncomment to rotate by itself)
                GLfloat rad_disp = glm::radians(transform.orientation.x);

                // Compute object rotational matrix 
                glm::mat3 rot_mat{ glm::cos(rad_disp), glm::sin(rad_disp), 0,
                                  -glm::sin(rad_disp), glm::cos(rad_disp), 0,
                                  0, 0, 1 };

                // Compute object translation matrix
                glm::mat3 trans_mat{ 1, 0, 0,
                                     0, 1, 0,
                                     transform.position.x, transform.position.y, 1 };

                // Compute world scale matrix
                glm::mat3 world_scale{ 1.0 / DEFAULT_WORLD_RANGE, 0, 0,
                                       0, 1.0 / DEFAULT_WORLD_RANGE, 0,
                                       0, 0, 1 };

                // Compute model-to-world-to-NDC transformation matrix and store it in the graphics component
                graphics.mdl_to_ndc_xform = world_scale * trans_mat * rot_mat * scale_mat;
            }
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
        
        // Draw all objects
        Render_System::draw();
    }

    // Renders the entity onto the window based on the graphics components
    void Render_System::draw() {

        for (const auto& entity_ptr : ecs_manager.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Check if the entity has Graphics, Transform and Collision components 
            if (entity_ptr->has_component(ecs_manager.get_component_id<Graphics_Component>()) && 
                entity_ptr->has_component(ecs_manager.get_component_id<Transform2D>())) { 

                auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
                auto& transform = ecs_manager.get_component<Transform2D>(entity_id);        // For Debugging
                auto& velocity = ecs_manager.get_component<Velocity_Component>(entity_id);  // For Debugging 
                auto& collision = ecs_manager.get_component<Collision_Component>(entity_id);// For Debugging

                // Get shaders and models container 
                auto& shaders = GFXM.get_shader_program_storage();
                auto& models = GFXM.get_model_storage();

                // Start the shader program that the entity will use for rendering
                GFXM.program_use(shaders[graphics.shd_ref]);

                // Bind object's VAO handle
                glBindVertexArray(models[graphics.model_name].vaoid);

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
                if (GFXM.get_debug_mode() == GL_TRUE) { 
                    if (entity_id != 0) { // Background object unaffected               
                        std::vector<glm::mat3> box_mdl_to_ndc_xform;

                        // Compute mdl_to_ndc_xform for each line of AABB box
                        for (std::size_t i = 0; i < 4; ++i) {
                            GLfloat scale_width = collision.width, scale_height = collision.height;
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
                            glm::mat3 AABB_rot_mat{ glm::cos(AABB_rad_disp), glm::sin(AABB_rad_disp), 0,
                                                    -glm::sin(AABB_rad_disp), glm::cos(AABB_rad_disp), 0,
                                                    0, 0, 1 };

                            // Compute line translation matrix
                            glm::mat3 AABB_trans_mat{ 1, 0, 0,
                                                      0, 1, 0,
                                                      transform.position.x, transform.position.y, 1 };

                            // Compute world scale matrix
                            glm::mat3 world_scale{ 1.0 / DEFAULT_WORLD_RANGE, 0, 0,
                                                   0, 1.0 / DEFAULT_WORLD_RANGE, 0,
                                                   0, 0, 1 };

                            // Compute model-to-world-to-NDC transformation matrix for line and store it for the AABB box
                            glm::mat3 result_xform = world_scale * AABB_trans_mat * AABB_rot_mat * AABB_scale_mat;
                            box_mdl_to_ndc_xform.emplace_back(result_xform);
                        }

                        // Set draw color for AABB box and velocity line to black
                        glm::vec3 AABB_box_color{ 0.0f, 0.0f, 0.0f };
                        if (color_uniform_loc >= 0) {
                            glUniform3fv(color_uniform_loc, 1, &AABB_box_color[0]);
                        }

                        // Drawing AABB box line by line
                        for (std::size_t i = 0; i < 4; ++i) {
                            glLineWidth(DEFAULT_AABB_WIDTH); 
                            glUniformMatrix3fv(mat_uniform_loc, 1, GL_FALSE, &box_mdl_to_ndc_xform[i][0][0]);
                            glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);

                        }

                        // Set up to draw velocity line
                        glBindVertexArray(models["debug_line"].vaoid);

                        // Draw the line according to velocity
                        if (mat_uniform_loc >= 0) {

                            // Compute line scale matrix
                            glm::mat3 scale_mat{ transform.scale.x * DEFAULT_VELOCITY_LINE_LENGTH, 0, 0,
                                                 0, transform.scale.y * DEFAULT_VELOCITY_LINE_LENGTH, 0,
                                                 0, 0, 1 };

                            // compute current orientation of line
                            GLfloat direction{ 0.0f }; 
                            if (velocity.velocity.x && (velocity.velocity.y == 0.0f)) {
                                if (velocity.velocity.x > 0.0f) {
                                    direction = -DEFAULT_ROTATION; // Move right 
                                }
                                else {
                                    direction = DEFAULT_ROTATION; // Move left 
                                }
                            }
                            else if (velocity.velocity.y && (velocity.velocity.x == 0.0f)) {
                                if (velocity.velocity.y > 0.0f) {
                                    direction = 0.0f; // Move up 
                                }
                                else {
                                    direction = 2.0f * DEFAULT_ROTATION; // Move down 
                                }
                            }
                            else if (velocity.velocity.x > 0.0f && velocity.velocity.y > 0.0f) { // Top right
                                direction = -DEFAULT_ROTATION / 2.0f;
                            }
                            else if (velocity.velocity.x < 0.0f && velocity.velocity.y > 0.0f) { // Top left
                                direction = DEFAULT_ROTATION / 2.0f;
                            }
                            else if (velocity.velocity.x > 0.0f && velocity.velocity.y < 0.0f) { // Bottom right
                                direction = -DEFAULT_ROTATION * 1.5f;
                            }
                            else if (velocity.velocity.x < 0.0f && velocity.velocity.y < 0.0f) { // Bottom right
                                direction = DEFAULT_ROTATION * 1.5f;
                            }

                            // Convert line orientation to radians
                            GLfloat rad_disp = glm::radians(direction);

                            // Compute line rotational matrix 
                            glm::mat3 rot_mat{ glm::cos(rad_disp), glm::sin(rad_disp), 0,
                                              -glm::sin(rad_disp), glm::cos(rad_disp), 0,
                                              0, 0, 1 };

                            // Compute line translation matrix
                            glm::mat3 trans_mat{ 1, 0, 0,
                                                 0, 1, 0,
                                                 transform.position.x, transform.position.y, 1 };

                            // Compute world scale matrix
                            glm::mat3 world_scale{ 1.0 / DEFAULT_WORLD_RANGE, 0, 0,
                                                   0, 1.0 / DEFAULT_WORLD_RANGE, 0,
                                                   0, 0, 1 };

                            // Set line mdl_to_ndc_xform to the vertex shader
                            glm::mat3 line_mdl_to_ndc_xform = world_scale * trans_mat * rot_mat * scale_mat;
                            glUniformMatrix3fv(mat_uniform_loc, 1, GL_FALSE, &line_mdl_to_ndc_xform[0][0]);
                        }
                        glLineWidth(DEFAULT_LINE_WIDTH);
                        glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                    }
                }
                // Clean up by unbinding the VAO and ending the shader program
                glBindVertexArray(0);
                GFXM.program_free();
            }
        }
    }

} // namespace lof
