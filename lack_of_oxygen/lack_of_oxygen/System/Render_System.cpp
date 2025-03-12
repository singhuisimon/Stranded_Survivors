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
#include "../Manager/Game_Manager.h"
#include "../Utility/globals.h"    // To access level_editor_mode

namespace lof {

    Render_System::Render_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Graphics_Component>());
        signature.set(ECSM.get_component_id<Transform2D>());
    }

    std::string Render_System::get_type() const {
        return "Render_System";
    }

    // Updates the model-to-world-to-NDC transformation matrix of the entity per frame.sti
    void Render_System::update(float delta_time) {

        // Get screen width and height
        GLfloat screen_width = static_cast<GLfloat>(SM.get_scr_width());
        GLfloat screen_height = static_cast<GLfloat>(SM.get_scr_height());

        // Update objects position with reference to camera type
        auto& camera = GFXM.get_camera();

        // Get the current scene number
        int current_scene = GM.get_current_scene();

        // Set up iterators for entities
        auto& all_entities = get_entities();
        auto start = all_entities.begin();
        auto end = all_entities.end();

        // Access player's ID
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        // Separate UI entities from other entities in scene 2
        if (current_scene == 2) {

            // Find position of ui_overlay to indicate end of first iteration of objects
            EntityID ui_overlay = ECSM.find_entity_by_name("top_ui_overlay");
            size_t entities_size = all_entities.size();
            if (ui_overlay != INVALID_ENTITY_ID) {
                end = std::prev(end, entities_size - ui_overlay);
            }
        }

        // Loop over the entities that match the system's signature
        for (; start != end; ++start) {
            auto& graphics = ECSM.get_component<Graphics_Component>(*start);
            auto& transform = ECSM.get_component<Transform2D>(*start);

            if (*start != 0) { // Background object unaffected

                // Scaling update when up or down arrow key pressed
                GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);

                // Check if the entity has Collision_Component
                if (ECSM.has_component<Collision_Component>(*start)) {

                    auto& collision = ECSM.get_component<Collision_Component>(*start);

                    int scale_flag = GFXM.get_scale_flag();
                    if (scale_flag == GLFW_KEY_UP) {
                        LM.write_log("Render_System::update(): 'UP' key held, increasing scale of entity %u by %f.", *start, scale_change);
                        transform.scale.x += scale_change;
                        transform.scale.y += scale_change;
                        collision.width += scale_change;
                        collision.height += scale_change;
                    }
                    else if (scale_flag == GLFW_KEY_DOWN) {
                        LM.write_log("Render_System::update(): 'DOWN' key held, decreasing scale of entity %u by %f.", *start, scale_change);
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
                        LM.write_log("Render_System::update(): 'LEFT' key held, rotating entity %u by %f.", *start, rot_change);
                    }
                    else if (rotation_flag == GLFW_KEY_RIGHT) {
                        GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                        transform.orientation.x -= rot_change;
                        LM.write_log("Render_System::update(): 'RIGHT' key held, rotating entity %u by %f.", *start, rot_change);
                    }

                }

            }

            // Update camera bounded to player
            if (*start == player_id && camera.is_free_cam == GL_FALSE) {

                // Update world-to-camera view transformation matrix
                camera.pos_y = transform.position.y;
                camera.view_xform = glm::mat3{ 1, 0, 0,
                                               0, 1, 0,
                                               -1, -transform.position.y, 1 };

                // Update window-to-NDC transformation matrix
                camera.camwin_to_ndc_xform = glm::mat3{ 2.f / screen_width, 0, 0,
                                                       0, 2.f / screen_height, 0,
                                                       0, 0, 1 };

                // Update world-to-NDC transformation matrix
                camera.world_to_ndc_xform = camera.camwin_to_ndc_xform * camera.view_xform;
            }
            else if (camera.is_free_cam == GL_TRUE) {

                // Update world-to-camera view transformation matrix
                camera.view_xform = glm::mat3{ 1, 0, 0,
                                               0, 1, 0,
                                               -camera.pos_x, -camera.pos_y, 1 };

                // Update window-to-NDC transformation matrix
                camera.camwin_to_ndc_xform = glm::mat3{ 2.f / screen_width, 0, 0,
                                                       0, 2.f / screen_height, 0,
                                                       0, 0, 1 };

                // Update world-to-NDC transformation matrix
                camera.world_to_ndc_xform = camera.camwin_to_ndc_xform * camera.view_xform;
            }

            if (camera.is_free_cam == GL_FALSE && current_scene != 1 && current_scene != 2) {

                // Update world-to-camera view transformation matrix
                camera.view_xform = glm::mat3{ 1, 0, 0,
                                               0, 1, 0,
                                               0, 0, 1 };

                // Update window-to-NDC transformation matrix
                camera.camwin_to_ndc_xform = glm::mat3{ 2.f / screen_width, 0, 0,
                                                       0, 2.f / screen_height, 0,
                                                       0, 0, 1 };

                // Update world-to-NDC transformation matrix
                camera.world_to_ndc_xform = camera.camwin_to_ndc_xform * camera.view_xform;
            }


            // Compute object scale matrix
            // Special case for text objects
            float scale_x{ 0 }, scale_y{ 0 }, translate_x{ 0 }, translate_y{ 0 };
            if (ECSM.has_component<Text_Component>(*start)) {
                // Get text component's scaling factor
                auto& text = ECSM.get_component<Text_Component>(*start);
                scale_x = text.scale.x;
                scale_y = text.scale.y;
                translate_x = transform.position.x - (transform.position.x * text.scale.x);  // Translate text back to original position based on scale
                translate_y = transform.position.y - (transform.position.y * text.scale.y);
            }
            else {
                scale_x = transform.scale.x;
                scale_y = transform.scale.y;
                translate_x = transform.position.x;
                translate_y = transform.position.y;
            }

            glm::mat3 scale_mat{ scale_x, 0, 0,
                                 0, scale_y, 0,
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
                                    translate_x, translate_y, 1 };

            graphics.mdl_to_ndc_xform = camera.world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
        }

        // Update the UI entities 
        if (current_scene == 2) {
            start = end;
            end = all_entities.end();

            // Store start index of UIs
            UI_start = *start;

            // UI entities own world to ndc xform
            glm::mat3 ui_view_xform = glm::mat3{ 1, 0, 0,
                                           0, 1, 0,
                                           -1, 102, 1 };

            glm::mat3 ui_win_to_ndc_xform = glm::mat3{ 2.f / screen_width, 0, 0,
                                                   0, 2.f / screen_height, 0,
                                                   0, 0, 1 };

            glm::mat3 ui_world_to_ndc_xform = ui_win_to_ndc_xform * ui_view_xform;

            for (; start != end; ++start) {

                auto& graphics = ECSM.get_component<Graphics_Component>(*start);
                auto& transform = ECSM.get_component<Transform2D>(*start);

                //// Scaling update when up or down arrow key pressed
                //GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);

                //// Check if the entity has Collision_Component
                //if (ECSM.has_component<Collision_Component>(*start)) {

                //    auto& collision = ECSM.get_component<Collision_Component>(*start);

                //    int scale_flag = GFXM.get_scale_flag();
                //    if (scale_flag == GLFW_KEY_UP) {
                //        LM.write_log("Render_System::update(): 'UP' key held, increasing scale of entity %u by %f.", *start, scale_change);
                //        transform.scale.x += scale_change;
                //        transform.scale.y += scale_change;
                //        collision.width += scale_change;
                //        collision.height += scale_change;
                //    }
                //    else if (scale_flag == GLFW_KEY_DOWN) {
                //        LM.write_log("Render_System::update(): 'DOWN' key held, decreasing scale of entity %u by %f.", *start, scale_change);
                //        if (transform.scale.x > 0.0f) {
                //            transform.scale.x -= scale_change;
                //            collision.width -= scale_change;
                //        }
                //        else {
                //            transform.scale.x = 0.0f;
                //            collision.width = 0.0f;
                //        }

                //        if (transform.scale.y > 0.0f) {
                //            transform.scale.y -= scale_change;
                //            collision.height -= scale_change;
                //        }
                //        else {
                //            transform.scale.y = 0.0f;
                //            collision.height = 0.0f;
                //        }
                //    }

                //    // Rotation update when left or right arrow key pressed
                //    int rotation_flag = GFXM.get_rotation_flag();
                //    if (rotation_flag == GLFW_KEY_LEFT) {
                //        GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                //        transform.orientation.x += rot_change;
                //        LM.write_log("Render_System::update(): 'LEFT' key held, rotating entity %u by %f.", *start, rot_change);
                //    }
                //    else if (rotation_flag == GLFW_KEY_RIGHT) {
                //        GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
                //        transform.orientation.x -= rot_change;
                //        LM.write_log("Render_System::update(): 'RIGHT' key held, rotating entity %u by %f.", *start, rot_change);
                //    }

                //}

                // Compute object scale matrix
                // Special case for text objects
                float scale_x{ 0 }, scale_y{ 0 }, translate_x{ 0 }, translate_y{ 0 };
                if (ECSM.has_component<Text_Component>(*start)) {
                    // Get text component's scaling factor
                    auto& text = ECSM.get_component<Text_Component>(*start);
                    scale_x = text.scale.x;
                    scale_y = text.scale.y;
                    translate_x = transform.position.x - (transform.position.x * text.scale.x);  // Translate text back to original position based on scale
                    translate_y = transform.position.y - (transform.position.y * text.scale.y);
                }
                else {
                    scale_x = transform.scale.x;
                    scale_y = transform.scale.y;
                    translate_x = transform.position.x;
                    translate_y = transform.position.y;
                }

                glm::mat3 scale_mat{ scale_x, 0, 0,
                                     0, scale_y, 0,
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
                                        translate_x, translate_y, 1 };

                graphics.mdl_to_ndc_xform = ui_world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
            }

        }

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

        // Set up the imgui framebuffer when entering editor mode
        if (GFXM.get_editor_mode() == 1) {
            glBindFramebuffer(GL_FRAMEBUFFER, GFXM.get_framebuffer());
        }

        // Set up for the drawing of objects
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw all objects
        draw();
    }

    // Renders the entity onto the window based on the graphics components
    void Render_System::draw() {

        // Get screen height
        GLfloat screen_height = static_cast<GLfloat>(SM.get_scr_height());

        // Get models and textures
        auto& models = GFXM.get_models();
        auto& textures = ASM.get_texture_storage();

        // Get camera
        auto& camera = GFXM.get_camera();

        // Access IDs for player and lava
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME); 
        EntityID lava_pool = ECSM.find_entity_by_name("lava_pool");

        // Get wormhole IDs and add background to it
        auto wormholes = SM.get_wormholes_id();
        wormholes.insert(wormholes.begin(), 0);

        // Loop over the wormhole entities
        for (auto itr = wormholes.begin(); itr != wormholes.end(); ++itr) {
            //for (EntityID entity_id : get_entities()) {

            auto& graphics = ECSM.get_component<Graphics_Component>(*itr);
            auto& transform = ECSM.get_component<Transform2D>(*itr);

            // Render only what is on the viewport
            int current_scene = GM.get_current_scene(); // Get the current scene number
            if (camera.is_free_cam == GL_FALSE && current_scene == 2) {
                if (*itr != 0) { // Skip background
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float render_boundary_top = player_transform.position.y + (screen_height * 0.6f);
                    float render_boundary_bottom = player_transform.position.y - (screen_height * 0.6f);

                    // Skip if object not in viewport
                    if (transform.position.y > render_boundary_top || transform.position.y < render_boundary_bottom) {
                        continue;
                    }
                }
            }

            // Get shader program
            Assets_Manager::ShaderProgram* shader = ASM.get_shader_program(graphics.shd_ref);

            // Start the shader program that the entity will use for rendering 
            GFXM.program_use(shader->program_handle);

            // Bind object's VAO handle
            glBindVertexArray(models[graphics.model_name].vaoid);

            // Check if entity has a texture
            if (graphics.texture_name != DEFAULT_TEXTURE_NAME) {

                ASM.track_entity_asset(*itr, "Graphics_Component", graphics.texture_name);
                // Look for texture in texture storage. If not found, load texture 
                if (textures.find(graphics.texture_name) == textures.end()) {
                    GFXM.load_texture(graphics.texture_name);
                }

                // Assign texture object to use texture image unit 5. If texture
                // is not loaded, render with default black texture
                if (textures.find(graphics.texture_name) == textures.end()) {
                    glBindTextureUnit(5, 0);
                }
                else {
                    glBindTextureUnit(5, textures[graphics.texture_name]);
                }

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

                // If entity has animation that is not default, pass animation data to fragment shader
                bool has_animation = ECSM.has_component<Animation_Component>(*itr);
                if (has_animation == true) {

                    // Get animation storage
                    auto& animations = ASM.get_animation_storage();

                    auto& animation = ECSM.get_component<Animation_Component>(*itr);
                    std::string const& curr_animation_name = animation.animations[std::to_string(animation.curr_animation_idx)];
                    if ((curr_animation_name != DEFAULT_ANIMATION_NAME) && (animations.find(curr_animation_name) != animations.end())) {
                        // Set animation flag to be true
                        GLuint animate_flag_true_loc = glGetUniformLocation(shader->program_handle, "uAnimateFlag");
                        if (animate_flag_true_loc >= 0) {
                            glUniform1ui(animate_flag_true_loc, GL_TRUE);
                        }
                        else {
                            LM.write_log("Render_System::draw(): Animation flag boolean doesn't exist.");
                            std::exit(EXIT_FAILURE);
                        }

                        // Pass frame number of current frame
                        GLuint frame_no_loc = glGetUniformLocation(shader->program_handle, "uFrameNo");
                        if (frame_no_loc >= 0) {
                            glUniform1i(frame_no_loc, animations[curr_animation_name].frames[animation.curr_frame_index].frame_number);
                        }
                        else {
                            LM.write_log("Render_System::draw(): Frame number value doesn't exist.");
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
                glUniform4fv(color_uniform_loc, 1, &graphics.color[0]);
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
            if (*itr == 0) { // Set background object to always render in fill mode
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(models[graphics.model_name].primitive_type, models[graphics.model_name].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                glPolygonMode(GL_FRONT_AND_BACK, GFXM.get_render_mode());
            }
            else {
                glDrawElements(models[graphics.model_name].primitive_type, models[graphics.model_name].draw_cnt, GL_UNSIGNED_SHORT, NULL);
            }

#if _DEBUG
            // Draw debugging features if debug mode is ON 
            if (*itr != 0) { // Background object unaffected 
                if (GFXM.get_debug_mode() == GL_TRUE) {

                    // Check if entity has Velocity_Component and Collision_Component
                    bool has_velocity = ECSM.has_component<Velocity_Component>(*itr);
                    bool has_collision = ECSM.has_component<Collision_Component>(*itr);

                    if (has_velocity || has_collision) {

                        // Free texture shader program to allow models shader program to bind and start  
                        GFXM.program_free();
                        int safe_shd_ref = static_cast<int>(graphics.shd_ref) + 1; // prevent overflow
                        Assets_Manager::ShaderProgram* debug_shader = ASM.get_shader_program(safe_shd_ref);
                        GFXM.program_use(debug_shader->program_handle);

                        // Set draw color for debug shapes to black and pass to fragment shader uniform variable uColor
                        GLint debug_color_uniform_loc = glGetUniformLocation(debug_shader->program_handle, "uColor");
                        glm::vec4 debug_color{ 0.0f, 0.0f, 0.0f, 1.0f };
                        if (debug_color_uniform_loc >= 0) {
                            glUniform4fv(debug_color_uniform_loc, 1, &debug_color[0]);
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
                            auto& collision = ECSM.get_component<Collision_Component>(*itr);

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
                            auto& velocity = ECSM.get_component<Velocity_Component>(*itr);

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
#endif
            // Clean up by unbinding the VAO and ending the shader program
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GFXM.program_free();
        }

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {

            // Skip background
            if (entity_id == 0) { continue; }

            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);

            // Render only what is on the viewport
            int current_scene = GM.get_current_scene(); // Get the current scene number
            if (camera.is_free_cam == GL_FALSE && current_scene == 2) {
                if (entity_id != 0 && entity_id != player_id && 
                    lava_pool != INVALID_ENTITY_ID && entity_id != lava_pool 
                    && (entity_id < UI_start)) {
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float render_boundary_top = player_transform.position.y + (screen_height * 0.6f);
                    float render_boundary_bottom = player_transform.position.y - (screen_height * 0.6f);

                    // Skip if object not in viewport
                    if (transform.position.y > render_boundary_top || transform.position.y < render_boundary_bottom) {
                        continue;
                    }

                    // Skip wormholes, they are already rendered
                    if (std::find(wormholes.begin(), wormholes.end(), entity_id) != wormholes.end()) {
                        continue;
                    }
                }
            }

            // Get shader program
            Assets_Manager::ShaderProgram* shader = ASM.get_shader_program(graphics.shd_ref);

            // Check for text objects to render 
            bool is_text = ECSM.has_component<Text_Component>(entity_id);
            if (is_text == true) {

                auto& text_comp = ECSM.get_component<Text_Component>(entity_id);
                auto& fonts = ASM.get_font_storage();

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
                GLuint text_mat_uniform_loc = glGetUniformLocation(shader->program_handle, "uModel_to_NDC_Mat");
                if (text_mat_uniform_loc >= 0) {
                    glUniformMatrix3fv(text_mat_uniform_loc, 1, GL_FALSE, &graphics.mdl_to_ndc_xform[0][0]);
                    //glUniformMatrix3fv(text_mat_uniform_loc, 1, GL_FALSE, &final_xform[0][0]);
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
                float scale_x = transform.position.x;
                float scale_y = 0;
                for (c = text_comp.text.begin(); c != text_comp.text.end(); c++)
                {
                    // Get read-only values from character
                    auto const& bearing = fonts[text_comp.font_name].characters[*c].Bearing;
                    auto const& size = fonts[text_comp.font_name].characters[*c].Size;
                    auto const& texture_id = fonts[text_comp.font_name].characters[*c].TextureID;
                    auto const& advance = fonts[text_comp.font_name].characters[*c].Advance;

                    // Get largest glpyh for scale factor 
                    scale_y = scale_y < size.y ? size.y : scale_y;

                    // Calculate the position and size of character in world 
                    float xpos = base_x - ((transform.scale.x / text_comp.scale.x) / 2.0f) + bearing.x;
                    float ypos = transform.position.y - ((transform.scale.y / text_comp.scale.y) / 2.0f) - (size.y - bearing.y);

                    //LM.write_log("Render_System::draw(): Font glyph size check: x - %u, y - %u.", size.x, size.y);

                    // Update VBO for each character
                    float vertices[6][4] = {
                        { xpos,          ypos + size.y,   0.0f, 0.0f },
                        { xpos,          ypos,            0.0f, 1.0f },
                        { xpos + size.x, ypos,            1.0f, 1.0f },
                        { xpos,          ypos + size.y,   0.0f, 0.0f },
                        { xpos + size.x, ypos,            1.0f, 1.0f },
                        { xpos + size.x, ypos + size.y,   1.0f, 0.0f }
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
                    base_x += (advance >> 6);

                }
                // Free VAO, texture id, and program once rendering completed
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);
                GFXM.program_free();

                // Set the scale of text object
                transform.scale.x = (base_x - scale_x) * text_comp.scale.x;
                transform.scale.y = scale_y * text_comp.scale.y;

                // Skip other rendering operations
                continue;
            }

            // Start the shader program that the entity will use for rendering 
            GFXM.program_use(shader->program_handle);

            // Bind object's VAO handle
            glBindVertexArray(models[graphics.model_name].vaoid);

            // Check if entity has a texture
            if (graphics.texture_name != DEFAULT_TEXTURE_NAME) {

                ASM.track_entity_asset(entity_id, "Graphics_Component", graphics.texture_name);
                // Look for texture in texture storage. If not found, load texture 
                if (textures.find(graphics.texture_name) == textures.end()) {
                    GFXM.load_texture(graphics.texture_name);
                }

                // Assign texture object to use texture image unit 5. If texture
                // is not loaded, render with default black texture
                if (textures.find(graphics.texture_name) == textures.end()) {
                    glBindTextureUnit(5, 0);
                }
                else {
                    glBindTextureUnit(5, textures[graphics.texture_name]);
                }

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


                // If entity has animation that is not default, pass animation data to fragment shader
                bool has_animation = ECSM.has_component<Animation_Component>(entity_id);
                if (has_animation == true) {

                    // Get animation storage
                    auto& animations = ASM.get_animation_storage();

                    auto& animation = ECSM.get_component<Animation_Component>(entity_id);
                    std::string const& curr_animation_name = animation.animations[std::to_string(animation.curr_animation_idx)];
                    if ((curr_animation_name != DEFAULT_ANIMATION_NAME) && (animations.find(curr_animation_name) != animations.end())) {
                        // Set animation flag to be true
                        GLuint animate_flag_true_loc = glGetUniformLocation(shader->program_handle, "uAnimateFlag");
                        if (animate_flag_true_loc >= 0) {
                            glUniform1ui(animate_flag_true_loc, GL_TRUE);
                        }
                        else {
                            LM.write_log("Render_System::draw(): Animation flag boolean doesn't exist.");
                            std::exit(EXIT_FAILURE);
                        }

                        // Pass frame number of current frame
                        GLuint frame_no_loc = glGetUniformLocation(shader->program_handle, "uFrameNo");
                        if (frame_no_loc >= 0) {
                            if (curr_animation_name == "vent_strip_up" || curr_animation_name == "vent_strip_left" ||
                                curr_animation_name == "vent_strip_right" || curr_animation_name == "lava_animate") {
                                glUniform1i(frame_no_loc, animations[curr_animation_name].frames[animations[curr_animation_name].curr_frame_index].frame_number);
                            }
                            else {
                                glUniform1i(frame_no_loc, animations[curr_animation_name].frames[animation.curr_frame_index].frame_number);
                            }
                        }
                        else {
                            LM.write_log("Render_System::draw(): Frame number value doesn't exist.");
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
                glUniform4fv(color_uniform_loc, 1, &graphics.color[0]);
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
            glDrawElements(models[graphics.model_name].primitive_type, models[graphics.model_name].draw_cnt, GL_UNSIGNED_SHORT, NULL);
#if _DEBUG
            // Draw debugging features if debug mode is ON 
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
                    GLint debug_color_uniform_loc = glGetUniformLocation(debug_shader->program_handle, "uColor");
                    glm::vec4 debug_color{ 0.0f, 0.0f, 0.0f, 1.0f };
                    if (debug_color_uniform_loc >= 0) {
                        glUniform4fv(debug_color_uniform_loc, 1, &debug_color[0]);
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

                        // Compute model-to-world-to-NDC transformation matrix for the velocity line
                        glm::mat3 line_mdl_to_ndc_xform = camera.world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
                        glBindVertexArray(models["debug_line"].vaoid);
                        glLineWidth(DEFAULT_LINE_WIDTH);
                        glUniformMatrix3fv(debug_mat_uniform_loc, 1, GL_FALSE, &line_mdl_to_ndc_xform[0][0]);
                        glDrawElements(models["debug_line"].primitive_type, models["debug_line"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                    }
                }
            }

#endif
            // Clean up by unbinding the VAO and ending the shader program
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GFXM.program_free();
        }

        // Get particle system
        for (auto& system : ECSM.get_systems()) {
            if (system->get_type() == "Particle_System") {
                auto* particle_system = static_cast<Particle_System*>(system.get());
                if (!particle_system) {
                    LM.write_log("Game_Manager::update(): Fail to get particle system");
                    std::cerr << "Failed to get particle system" << std::endl;
                    return;
                }

                // Get number of active particles and the particle storage
                int particle_cnt = particle_system->get_particles_count();
                auto& particles_storage = particle_system->get_particle_storage();

                // Break loop if no particles to render
                if (particle_cnt < 0) {
                    break;
                }

                // Get shader program 
                Assets_Manager::ShaderProgram* shader = ASM.get_shader_program(0);

                // Start shader program
                GFXM.program_use(shader->program_handle);

                // Bind VAO handle
                glBindVertexArray(models["square"].vaoid);

                // Set texture flag to true
                GLuint tex_flag_true_loc = glGetUniformLocation(shader->program_handle, "uTexFlag");
                if (tex_flag_true_loc >= 0) {
                    glUniform1ui(tex_flag_true_loc, GL_TRUE);
                }
                else {
                    LM.write_log("Render_System::draw(): Texture flag uniform variable doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }

                // Set animation flag to be false
                GLuint animate_flag_false_loc = glGetUniformLocation(shader->program_handle, "uAnimateFlag");
                if (animate_flag_false_loc >= 0) {
                    glUniform1ui(animate_flag_false_loc, GL_FALSE);
                }
                else {
                    LM.write_log("Render_System::draw(): Animation flag boolean doesn't exist.");
                    std::exit(EXIT_FAILURE);
                }


                // Container for particle texture
                std::string particle_tex{};

                // Looping through each particles
                for (int i = 0; i < particle_cnt; ++i) {

                    // Set the texture for the type of particle
                    switch (particles_storage[i].type) {
                    case walking:
                        particle_tex = "dirt_particle_batch_14";
                        break;
                    case mining:
                        particle_tex = "sparks_particle_batch_14";
                        break;
                    case rock:
                        particle_tex = "rock_particle_batch_14";
                        break;
                    case dirt:
                        particle_tex = "dirt_particle_batch_14";
                        break;
                    case quartz:
                        particle_tex = "quartz_particle_batch_14";
                        break;
                    case emerald:
                        particle_tex = "emerald_particle_batch_14";
                        break;
                    case sapphire:
                        particle_tex = "sapphire_particle_batch_14";
                        break;
                    case amethyst:
                        particle_tex = "amethyst_particle_batch_14";
                        break;
                    case citrine:
                        particle_tex = "citrine_particle_batch_14";
                        break;
                    case alexandrite:
                        particle_tex = "alexandrite_particle_batch_14";
                        break;
                    case tnt:
                        particle_tex = "sparks_particle_batch_14";
                        break;
                    case tnt_explode:
                        particle_tex = "sparks_particle_batch_14";
                        break;
                    case tnt_vfx:
                        particle_tex = "sparks_red_particle_batch_14";
                        break;
                    case lava:
                        particle_tex = "sparks_particle_batch_14";
                        break;
                    }

                    // Look for texture in texture storage. If not found, load texture 
                    if (textures.find(particle_tex) == textures.end()) {
                        GFXM.load_texture(particle_tex);
                    }

                    // Assign texture object to use texture image unit 5. If texture
                    // is not loaded, render with default black texture
                    if (textures.find(particle_tex) == textures.end()) {
                        glBindTextureUnit(5, 0);
                    }
                    else {
                        glBindTextureUnit(5, textures[particle_tex]);
                    }

                    // Get camera for transform
                    auto& camera = GFXM.get_camera();

                    // Compute particle scale matrix
                    glm::mat3 scale_mat{ particles_storage[i].curr_size * 64, 0, 0,
                                        0, particles_storage[i].curr_size * 64, 0,
                                        0, 0, 1 };

                    // Compute particle translation matrix
                    glm::mat3 trans_mat{ 1, 0, 0,
                                            0, 1, 0,
                                            particles_storage[i].position.x, particles_storage[i].position.y, 1 };

                    // Compute final particles transform matrix
                    glm::mat3 particles_xform = camera.world_to_ndc_xform * trans_mat * scale_mat; 

                    if (particles_storage[i].type == lava) { 
                        // Compute current orientation of particle
                        GLfloat rad_disp = glm::radians(particles_storage[i].direction);

                        // Compute object rotational matrix 
                        glm::mat3 rot_mat{ glm::cos(rad_disp),  glm::sin(rad_disp), 0,
                                            -glm::sin(rad_disp),  glm::cos(rad_disp), 0,
                                            0,                   0,                  1 };

                        particles_xform = camera.world_to_ndc_xform * trans_mat * rot_mat * scale_mat;
                    }

                    // Pass particle's mdl_to_ndc_xform to vertex shader to compute object's final position                    
                    GLint mat_uniform_loc = glGetUniformLocation(shader->program_handle, "uModel_to_NDC_Mat");
                    if (mat_uniform_loc >= 0) {
                        glUniformMatrix3fv(mat_uniform_loc, 1, GL_FALSE, &particles_xform[0][0]);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Matrix uniform variable doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }

                    // Pass particle's color to fragment shader uniform variable uColor
                    GLint color_uniform_loc = glGetUniformLocation(shader->program_handle, "uColor");
                    if (color_uniform_loc >= 0) {
                        // Converting Vec3D to glm::vec4
                        glm::vec4 color_vec4 = { particles_storage[i].color.x, particles_storage[i].color.y, particles_storage[i].color.z, 255.0f};
                        glUniform4fv(color_uniform_loc, 1, &color_vec4[0]);
                    }
                    else {
                        LM.write_log("Render_System::draw(): Color uniform variable doesn't exist.");
                        std::exit(EXIT_FAILURE);
                    }

                    // Draw particle
                    glDrawElements(models["square"].primitive_type, models["square"].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                }

            }

            // Clean up by unbinding the VAO and ending the shader program
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GFXM.program_free();
        }

        // Bind framebuffer if program is in editor mode
        if (GFXM.get_editor_mode() == 1) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
} // namespace lof
