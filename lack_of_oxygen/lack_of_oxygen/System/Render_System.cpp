
#if 1
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

        // Get screen width and height
        GLfloat screen_width = static_cast<GLfloat>(SM.get_scr_width());
        GLfloat screen_height = static_cast<GLfloat>(SM.get_scr_height());

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);

            // Access player's ID
            EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

            if (entity_id != 0) { // Background object unaffected

                // Scaling update when up or down arrow key pressed
                GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);

                // Check if the entity has Collision_Component
                if (ECSM.has_component<Collision_Component>(entity_id)) {

                    auto& collision = ECSM.get_component<Collision_Component>(entity_id);

                    int scale_flag = GFXM.get_scale_flag();
                    if (scale_flag == GLFW_KEY_UP) {
                        LM.write_log("Render_System::update(): 'UP' key held, increasing scale of entity %u by %f.", entity_id, scale_change);
                        transform.scale.x += scale_change;
                        transform.scale.y += scale_change;
                        collision.width += scale_change;
                        collision.height += scale_change;
                    }
                    else if (scale_flag == GLFW_KEY_DOWN) {
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

                    // Rotation update when left or right arrow key pressed
                    int rotation_flag = GFXM.get_rotation_flag();
                    if (rotation_flag == GLFW_KEY_LEFT) {
                        GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                        transform.orientation.x += rot_change;
                        LM.write_log("Render_System::update(): 'LEFT' key held, rotating entity %u by %f.", entity_id, rot_change);
                    }
                    else if (rotation_flag == GLFW_KEY_RIGHT) {
                        GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                        transform.orientation.x -= rot_change;
                        LM.write_log("Render_System::update(): 'RIGHT' key held, rotating entity %u by %f.", entity_id, rot_change);
                    }
                }

            }

            // Update camera bounded to player
            auto& camera = GFXM.get_camera();

            if (entity_id == player_id && camera.is_free_cam == GL_FALSE) {

                // Update world-to-camera view transformation matrix
                camera.pos_y = transform.position.y;
                camera.view_xform = glm::mat3{ 1, 0, 0,
                                               0, 1, 0,
                                               -1, -transform.position.y, 1 };

                // Update window-to-NDC transformation matrix
                camera.camwin_to_ndc_xform = glm::mat3{ 1.f / (screen_width/2), 0, 0,
                                                       0, 1.f / (screen_height/2), 0,
                                                       0, 0, 1 };

                // Update world-to-NDC transformation matrix
                camera.world_to_ndc_xform = camera.camwin_to_ndc_xform * camera.view_xform;
            }
            else if (camera.is_free_cam == GL_TRUE) {

                //// Movement update when keypad 8 or 2 pressed
                //if (
                // is_key_held(GLFW_KEY_KP_8)) {
                //    camera.pos_y += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                //    LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
                //}
                //else if (IM.is_key_held(GLFW_KEY_KP_2)) {
                //    camera.pos_y -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                //    LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
                //}

                // Update world-to-camera view transformation matrix
                camera.view_xform = glm::mat3{ 1, 0, 0,
                                               0, 1, 0,
                                               -camera.pos_x, -camera.pos_y, 1 };

                // Update window-to-NDC transformation matrix
                camera.camwin_to_ndc_xform = glm::mat3{ 1.f / (screen_width/2), 0, 0,
                                                       0, 1.f / (screen_height/2), 0,
                                                       0, 0, 1 };

                // Update world-to-NDC transformation matrix
                camera.world_to_ndc_xform = camera.camwin_to_ndc_xform * camera.view_xform;
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
            glm::mat3 world_scale{ 1.0f / screen_width, 0, 0,
                                    0, 1.0f / screen_height, 0,
                                    0, 0, 1 };

            graphics.mdl_to_ndc_xform = camera.world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
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

        // Get screen width and height
        GLfloat screen_width = static_cast<GLfloat>(SM.get_scr_width());
        GLfloat screen_height = static_cast<GLfloat>(SM.get_scr_height());

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);


            // Get shaders, models, textures, animation, and camera from the Graphics Manager
            Assets_Manager::ShaderProgram* shader = ASM.get_shader_program(graphics.shd_ref);
            auto& models = GFXM.get_model_storage();
            auto& textures = GFXM.get_texture_storage();
            auto& animations = GFXM.get_animation_storage();
            auto& camera = GFXM.get_camera();

            // Check for text objects to render 
            bool is_text = ECSM.has_component<Text_Component>(entity_id);
            if (is_text == true) {

                auto& text_comp = ECSM.get_component<Text_Component>(entity_id);
                auto& fonts = GFXM.get_font_storage();

                // Start the shader program used for text rendering
                GFXM.program_use(shader->program_handle);

                // Set text color in fragment shader
                GLuint text_color_uniform_loc = glGetUniformLocation(shader->program_handle, "uTextColor");
                if (text_color_uniform_loc >= 0) {
                    glUniform3fv(text_color_uniform_loc, 1, &text_comp.color[0]);
                }
                else {
                    LM.write_log("Render_System::draw(): Text colour uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // Pass object's mdl_to_ndc_xform to vertex shader to compute object's final position
                GLint text_mat_uniform_loc = glGetUniformLocation(shader->program_handle, "uModel_to_NDC_Mat");
                if (text_mat_uniform_loc >= 0) {
                    glUniformMatrix3fv(text_mat_uniform_loc, 1, GL_FALSE, &graphics.mdl_to_ndc_xform[0][0]);
                }
                else {
                    LM.write_log("Render_System::draw(): Matrix uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // Set texture unit and bind text object's VAO handle 
                glActiveTexture(GL_TEXTURE0);
                glBindVertexArray(fonts[text_comp.font_name].vaoid);

                // Iterate through all characters
                std::string::const_iterator c;
                float base_x = transform.position.x;
                for (c = text_comp.text.begin(); c != text_comp.text.end(); c++)
                {
                    // Get read-only values from character
                    auto const& bearing = fonts[text_comp.font_name].characters[*c].Bearing;
                    auto const& size = fonts[text_comp.font_name].characters[*c].Size;
                    auto const& texture_id = fonts[text_comp.font_name].characters[*c].TextureID;
                    auto const& advance = fonts[text_comp.font_name].characters[*c].Advance;

                    // Calculate the position and size of character in world 
                    float xpos = base_x + bearing.x * transform.scale.x;
                    float ypos = transform.position.y - (size.y - bearing.y) * transform.scale.y;
                    float w = size.x * transform.scale.x;
                    float h = size.y * transform.scale.y;

                    // Update VBO for each character
                    float vertices[6][4] = {
                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos,     ypos,       0.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },
                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },
                        { xpos + w, ypos + h,   1.0f, 0.0f }
                    };

                    // Set texture id to render
                    glBindTexture(GL_TEXTURE_2D, texture_id);

                    // Update content of VBO memory and unbind once completed
                    glBindBuffer(GL_ARRAY_BUFFER, fonts[text_comp.font_name].vboid);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    // Render quad
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    
                    // Advance cursors for next glyph 
                    base_x += (advance >> 6) * transform.scale.x;
                }
                // Free VAO, texture id, and program once rendering completed
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);
                GFXM.program_free();

                // Skip other rendering operations
                continue;
            }

            // Start the shader program that the entity will use for rendering 
            GFXM.program_use(shader->program_handle);

            // Bind object's VAO handle
            glBindVertexArray(models[graphics.model_name].vaoid);

            // Check if entity has a texture
            if (graphics.texture_name != DEFAULT_TEXTURE_NAME) {
                // Assign texture object to use texture image unit 5 
                glBindTextureUnit(5, textures[graphics.texture_name]);
                LM.write_log("Render_System::draw(): Texture name: %s.", graphics.texture_name.c_str());

                // Set texture flag to true
                GLuint tex_flag_true_loc = glGetUniformLocation(shader->program_handle, "uTexFlag");
                if (tex_flag_true_loc >= 0) {
                    glUniform1ui(tex_flag_true_loc, GL_TRUE);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture flag uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // Set texture unit in fragment shader
                GLuint tex_uniform_loc = glGetUniformLocation(shader->program_handle, "uTex2d");
                if (tex_uniform_loc >= 0) {
                    glUniform1i(tex_uniform_loc, 5);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // If entity has animation, pass animation data to fragment shader
                bool has_animation = ECSM.has_component<Animation_Component>(entity_id);
                if (has_animation == true) {

                    auto& animation = ECSM.get_component<Animation_Component>(entity_id);
                    std::string const& curr_animation_name = animation.animations[std::to_string(animation.curr_animation_idx)];
                    unsigned int& curr_frame_idx = animations[curr_animation_name].curr_frame_index;

                    // Set animation flag to be true
                    GLuint animate_flag_true_loc = glGetUniformLocation(shader->program_handle, "uAnimateFlag");
                    if (animate_flag_true_loc >= 0) {
                        glUniform1ui(animate_flag_true_loc, GL_TRUE);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Animation flag boolean doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }

                    // Pass texture width, height, and frame size
                    GLuint tex_w_uniform_loc = glGetUniformLocation(shader->program_handle, "uTex_W");
                    GLuint tex_h_uniform_loc = glGetUniformLocation(shader->program_handle, "uTex_H");
                    GLuint frame_size_uniform_loc = glGetUniformLocation(shader->program_handle, "uFrame_Size");
                    if (tex_w_uniform_loc >= 0 && tex_h_uniform_loc >= 0 && frame_size_uniform_loc >= 0) {
                        glUniform1f(tex_w_uniform_loc, animations[curr_animation_name].tex_w);
                        glUniform1f(tex_h_uniform_loc, animations[curr_animation_name].tex_h);
                        glUniform1f(frame_size_uniform_loc, animations[curr_animation_name].frames[curr_frame_idx].size);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Texture width, Texture height, and/or frame size uniform variable doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }

                    // Pass frame position
                    GLuint frame_pos_x_uniform_loc = glGetUniformLocation(shader->program_handle, "uPos_X");
                    GLuint frame_pos_y_uniform_loc = glGetUniformLocation(shader->program_handle, "uPos_Y");
                    if (frame_pos_x_uniform_loc >= 0 && frame_pos_y_uniform_loc >= 0) {
                        glUniform1f(frame_pos_x_uniform_loc, animations[curr_animation_name].frames[curr_frame_idx].uv_x);
                        glUniform1f(frame_pos_y_uniform_loc, animations[curr_animation_name].frames[curr_frame_idx].uv_y);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Frame x and y uniform variable doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }

                }
                else {
                    // Set animation flag to be false
                    GLuint animate_flag_false_loc = glGetUniformLocation(shader->program_handle, "uAnimateFlag");
                    if (animate_flag_false_loc >= 0) {
                        glUniform1ui(animate_flag_false_loc, GL_FALSE);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Animation flag boolean doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }
                }
            }
            else {
                // Set texture flag to false
                GLuint tex_flag_false_loc = glGetUniformLocation(shader->program_handle, "uTexFlag");
                if (tex_flag_false_loc >= 0) {
                    glUniform1ui(tex_flag_false_loc, GL_FALSE);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture flag uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }
            }

            // Pass object's color to fragment shader uniform variable uColor
            GLint color_uniform_loc = glGetUniformLocation(shader->program_handle, "uColor");
            if (color_uniform_loc >= 0) {
                glUniform3fv(color_uniform_loc, 1, &graphics.color[0]);
            }
            else {
                LM.write_log("Render_System::draw(): Color uniform variable doesn't exist.");
                std::exit(EXIT_FAILURE);
            }

            // Pass object's mdl_to_ndc_xform to vertex shader to compute object's final position
            GLint mat_uniform_loc = glGetUniformLocation(shader->program_handle, "uModel_to_NDC_Mat");
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

                        // Free texture shader program to allow models shader program to bind and start  
                        GFXM.program_free();
                        int safe_shd_ref = static_cast<int>(graphics.shd_ref) + 1; // prevent overflow
                        Assets_Manager::ShaderProgram* debug_shader = ASM.get_shader_program(safe_shd_ref);
                        GFXM.program_use(debug_shader->program_handle);

                        // Set draw color for debug shapes to black and pass to fragment shader uniform variable uColor
                        GLint debug_color_uniform_loc = glGetUniformLocation(shader->program_handle, "uColor");
                        glm::vec3 debug_color{ 0.0f, 0.0f, 0.0f };
                        if (debug_color_uniform_loc >= 0) {
                            glUniform3fv(debug_color_uniform_loc, 1, &debug_color[0]);
                        }
                        else {
                            LM.write_log("Render_System::draw(): Debug color uniform variable doesn't exist.");
                            std::exit(EXIT_FAILURE);
                        }

                        // Pass debug object's mdl_to_ndc_xform to vertex shader to compute final position
                        GLint debug_mat_uniform_loc = glGetUniformLocation(debug_shader->program_handle, "uModel_to_NDC_Mat");
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
                                glm::mat3 world_scale{ 1.0f / screen_width, 0, 0,
                                                       0, 1.0f / screen_height, 0,
                                                       0, 0, 1 };

                                // Compute model-to-world-to-NDC transformation matrix for line and store it
                                glm::mat3 result_xform = camera.world_to_ndc_xform * AABB_trans_mat * AABB_rot_mat * AABB_scale_mat;
                                box_mdl_to_ndc_xform.emplace_back(result_xform);

                                // Drawing AABB box line by line
                                glLineWidth(DEFAULT_AABB_WIDTH);
                                glUniformMatrix3fv(debug_mat_uniform_loc, 1, GL_FALSE, &box_mdl_to_ndc_xform[i][0][0]);
                                glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                            }
                        }

                        // Drawing velocity vector if entity has Velocity_Component
                        if (has_velocity) {
                            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);

                            // Compute line scale matrix
                            glm::mat3 scale_mat{ transform.scale.x * DEFAULT_VELOCITY_LINE_LENGTH, 0, 0,
                                                 0, transform.scale.y * DEFAULT_VELOCITY_LINE_LENGTH, 0,
                                                 0, 0, 1 };

                            // Compute current orientation of line
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
                            glm::mat3 world_scale{ 1.0f / screen_width, 0, 0,
                                                   0, 1.0f / screen_height, 0,
                                                   0, 0, 1 };

                            // Compute model-to-world-to-NDC transformation matrix for the velocity line
                            glm::mat3 line_mdl_to_ndc_xform = camera.world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
                            glBindVertexArray(models["debug_line"].vaoid);
                            glLineWidth(DEFAULT_LINE_WIDTH);
                            glUniformMatrix3fv(debug_mat_uniform_loc, 1, GL_FALSE, &line_mdl_to_ndc_xform[0][0]);
                            glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                        }
                    }
                }
            }
            // Clean up by unbinding the VAO and ending the shader program
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GFXM.program_free();
        }
    }

} // namespace lof

#endif
