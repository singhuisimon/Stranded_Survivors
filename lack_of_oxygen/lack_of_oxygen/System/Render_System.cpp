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
                auto& velocity= ecs_manager.get_component<Velocity_Component>(entity_id); 

                /////////////////////////FOR TESTING OF CHANGE IN SCALE/////////////////////////////////
                GLfloat scale_change = 2.5f;
                if (IM.is_key_held(GLFW_KEY_9)) {
                    LM.write_log("Render_System::update(): '9' key pressed, scale of entity %u increased by %f.", entity_id, scale_change);
                    transform.scale.x += scale_change;
                    transform.scale.y += scale_change;
                }
                else if (IM.is_key_held(GLFW_KEY_0)) {
                    LM.write_log("Render_System::update(): '0' key pressed, scale of entity %u decreased by %f.", entity_id, scale_change);
                    if (transform.scale.x > 0.0f || transform.scale.y > 0.0f) {
                        transform.scale.x -= scale_change;
                        transform.scale.y -= scale_change;
                    }
                    else {
                        transform.scale.x = 0.0f;
                        transform.scale.y = 0.0f;
                    }
                }
                ///////////////////////FOR TESTING OF CHANGE IN SCALE/////////////////////////////////


                ///////////////////////FOR TESTING OF CHANGE IN ROTATION/////////////////////////////////
                GLfloat rot_change = 30.0f * (GLfloat)delta_time;
                if (IM.is_key_held(GLFW_KEY_LEFT)) {
                    LM.write_log("Render_System::update(): 'LEFT' key pressed, rotation of entity %u increased by %f.", entity_id, rot_change);
                    transform.rotation += rot_change;
                }
                else if (IM.is_key_held(GLFW_KEY_RIGHT)) {
                    LM.write_log("Render_System::update(): 'RIGHT' key pressed, rotation of entity %u decreased by %f.", entity_id, rot_change);
                    transform.rotation -= rot_change;
                }
                ///////////////////////FOR TESTING OF CHANGE IN ROTATION/////////////////////////////////

                ///////////////////////FOR TESTING MOVEMENT/////////////////////////////////
                if (entity_id == 0) {
                    //GLfloat movement_speed = 100.0f * (GLfloat)delta_time;
                    velocity.velocity.x *delta_time;
                    velocity.velocity.y *  delta_time;
                    if (IM.is_key_held(GLFW_KEY_W)) {
                        LM.write_log("Render_System::update(): 'W' key pressed, entity %u move up by %f.", entity_id, velocity.velocity.y);
                        transform.position.y += velocity.velocity.y * delta_time;;
                    }
                    if (IM.is_key_held(GLFW_KEY_A)) {
                        LM.write_log("Render_System::update(): 'A' key pressed, entity %u left up by %f.", entity_id, velocity.velocity.x);
                        transform.position.x -= velocity.velocity.x * delta_time;;
                    }
                    if (IM.is_key_held(GLFW_KEY_S)) {
                        LM.write_log("Render_System::update(): 'S' key pressed, entity %u down up by %f.", entity_id, velocity.velocity.y);
                        transform.position.y -= velocity.velocity.y * delta_time;;
                    }
                    if (IM.is_key_held(GLFW_KEY_D)) {
                        LM.write_log("Render_System::update(): 'D' key pressed, entity %u right up by %f.", entity_id, velocity.velocity.x);
                        transform.position.x += velocity.velocity.x * delta_time;;
                    }
                }

                /////////////////////////FOR TESTING MOVEMENT/////////////////////////////////

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
                //LM.write_log("Render_System::update(): entity ID %u update successful. Updated position is %f, %f.", entity_id, transform.position.x, transform.position.y);
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
                }
                else {
                    LM.write_log("Render_System::draw(): Uniform variable doesn't exist!");
                    std::exit(EXIT_FAILURE);
                }

                // Render objects
                glDrawElements(models[graphics.mdl_ref].primitive_type, models[graphics.mdl_ref].draw_cnt, GL_UNSIGNED_SHORT, NULL);
                //LM.write_log("Render_System::draw(): entity ID %u rendered successfully at position %f, %f.", entity_id, transform.position.x, transform.position.y);

                // Clean up by unbinding the VAO and ending the shader program
                glBindVertexArray(0);
                GFXM.program_free();

            }
        }
    }

} // namespace lof
