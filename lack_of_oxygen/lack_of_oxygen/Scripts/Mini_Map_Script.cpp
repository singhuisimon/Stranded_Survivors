/**
 * @file Mini_Map_Script.cpp
 * @brief Declare the Mini_Map_Script class
 * @author Saw Hui Shan (100%)
 * @date March 25, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Mini_Map_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/Serialization_Manager.h"

namespace lof
{
	//Mini_Map_Script::Mini_Map_Script() {};

	std::string Mini_Map_Script::get_type() const 
	{
		return script_name; // return mini_map_script
	}

    void Mini_Map_Script::register_script() {
        auto mini_map_script = shared_from_this();

        mini_map_script->add_function("init", [weak_script = std::weak_ptr<Mini_Map_Script>(mini_map_script)](EntityID entity_id) {
            (void)entity_id;
            auto mini_map_script = weak_script.lock();
            //tutorial_light_script->init_light();
            });

        mini_map_script->add_function("update", [weak_script = std::weak_ptr<Mini_Map_Script>(mini_map_script)](EntityID entity_id) {
            (void)entity_id;
            auto mini_map_script = weak_script.lock();
            //mini_map_script->update_frame();
            //mini_map_script->lights_animation();
            mini_map_script->Mini_Map_UI_Lava();
            });

    }

    void Mini_Map_Script::Mini_Map_UI_Lava()
    {
        // to ensure game won't crash if there is no entity that is not support to be there
        if (GM.get_current_scene() != 2) {
            return;
        }

        EntityID meter_bar = ECSM.find_entity_by_name("mini_map_meter_bar");

        EntityID mini_lava_icon = ECSM.find_entity_by_name("mini_lava");
        EntityID lava_gauge = ECSM.find_entity_by_name("mini_map_lava_gauge");
        EntityID mini_lava_loc = ECSM.find_entity_by_name("mini_lava_loc");
        EntityID lava_pool = ECSM.find_entity_by_name("lava_pool"); // to get the transformation info of lava pool 

        EntityID mini_player_icon = ECSM.find_entity_by_name("mini_player");
        EntityID player_loc = ECSM.find_entity_by_name("player_loc");
        EntityID player = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        EntityID mini_map_player_gauge = ECSM.find_entity_by_name("mini_map_player_gauge");


        auto& meter_bar_transform = ECSM.get_component<Transform2D>(meter_bar);
        auto& mini_lava_block_transform = ECSM.get_component<Transform2D>(mini_lava_icon);
        auto& mini_lava_loc_transform = ECSM.get_component<Transform2D>(mini_lava_loc);
        auto& lava_pool_transform = ECSM.get_component<Transform2D>(lava_pool);
        auto& player_loc_transform = ECSM.get_component<Transform2D>(player_loc);
        auto& mini_player_icon_transform = ECSM.get_component<Transform2D>(mini_player_icon);
        auto& player_transform = ECSM.get_component<Transform2D>(player);
        auto& lava_gauge_transform = ECSM.get_component<Transform2D>(lava_gauge);
        auto& player_gauge_transform = ECSM.get_component<Transform2D>(mini_map_player_gauge);


        if (meter_bar == INVALID_ENTITY_ID || mini_lava_icon == INVALID_ENTITY_ID || lava_gauge == INVALID_ENTITY_ID ||
            mini_lava_loc == INVALID_ENTITY_ID || lava_pool == INVALID_ENTITY_ID || mini_player_icon == INVALID_ENTITY_ID ||
            player_loc == INVALID_ENTITY_ID || player == INVALID_ENTITY_ID || mini_map_player_gauge == INVALID_ENTITY_ID) // ensure it is exist 
        {
            return;
        }

        //const LevelData& current_level = SM.get_current_level();
        const float LEFT_BOUND = -960.0f;
        const float RIGHT_BOUND = 960.0f;
        float total_width = RIGHT_BOUND - LEFT_BOUND;
        size_t level_rows = SM.get_level_rows(); //79
        size_t level_cols = SM.get_level_cols();
        float tiles_size = total_width / level_cols; // 96
        float START_Y = -150.0f; //the map start at position -150 instead of 0.0f
        float map_bottom_y = START_Y - (level_rows * tiles_size); // bottom of the map -7734 (after adding another 48 (half of tile size) 
        float total_map_height = START_Y - map_bottom_y; //7584 // same as total tiles * total rows

        //================METER BAR================//
        //std::cout << "======meter bar data======" << "\n";
        float meter_bar_init_height = meter_bar_transform.scale.y; // 370
        float meter_bar_init_pos = meter_bar_transform.position.y;
        float meter_bar_top = meter_bar_init_pos + (meter_bar_init_height / 2.0f); //-204 -> -235 > different: 31
       // std::cout << "meter_bar_top: " << meter_bar_top << "\n";
        float meter_bar_bottom = meter_bar_init_pos - (meter_bar_init_height / 2.0f); //-574 -> -605 different: 31 
        //std::cout << "meter_bar_bottom: " << meter_bar_bottom << "\n";

        //================LAVA POOL================//
        float lava_pool_y = lava_pool_transform.position.y; // -10326
        float initial_lava_y = map_bottom_y - (27.0f * tiles_size); // -10326 
        float lava_pool_interact = lava_pool_y + (lava_pool_transform.scale.y / 2); // -9786
        static float OFFSET_METER_BAR_LAVA = 3.0f;
        //static float OFFSET_LAVA_GAUGE_BELOW = 2.0f;
        //std::cout << "lava_pool_interact: " << lava_pool_interact << "\n";
        float lava_max_y = START_Y;//-150
        static float default_lava_icon_pos = mini_lava_block_transform.position.y; //-600
        //static float default_lava_icon_height = mini_lava_block_transform.scale.y; //-600

        static float default_lava_gauge = lava_gauge_transform.scale.y;
        static float BELOW_METER_OFFSET = meter_bar_bottom - default_lava_icon_pos; //26

        if (lava_pool_interact >= map_bottom_y)
        {
            float lava_rise_amount = lava_pool_interact - map_bottom_y;
            float total_possible_rise = (map_bottom_y + (level_rows * tiles_size)) - initial_lava_y;
            float lava_rise_ratio = std::max(0.0f, std::min(lava_rise_amount / total_possible_rise, 1.0f));

            // Base starting position (adjust this to your exact desired initial height)
            const float lava_start_position = -505.0f;  // Your specified starting Y position
            const float position_offset = -2.0f;
            // Mini lava position calculation
            float mini_lava_position = lava_start_position + position_offset + (lava_rise_ratio * meter_bar_init_height);
            mini_lava_block_transform.position.y = mini_lava_position;
            mini_lava_loc_transform.position.y = mini_lava_block_transform.position.y;

            // Lava gauge calculation - starts at same base position
            float gauge_height = default_lava_gauge + (lava_rise_ratio * (meter_bar_init_height - default_lava_gauge));
            float gauge_position = lava_start_position + (gauge_height / 2.0f) - (default_lava_gauge / 2.0f);  // Center of the gauge
            lava_gauge_transform.scale.y = gauge_height;
            lava_gauge_transform.position.y = gauge_position;


        }
        else
        {
            // Calculate how far the lava is from reaching the map bottom
            float lava_distance_to_bottom = map_bottom_y - lava_pool_interact;
            //float total_initial_distance = 27.0f * tiles_size; // Distance from initial position to map bottom
            float total_initial_distance = map_bottom_y - initial_lava_y;
            // Calculate ratio (0 when lava is at initial position, 1 when reaches map bottom)
            float lava_ratio = 1.0f - std::max(0.0f, std::min(lava_distance_to_bottom / total_initial_distance, 1.0f));

            // The range we want the lava indicator to move in the meter bar (-605 to -502)
            float lava_indicator_range = -516.0f - (meter_bar_bottom); // 103 units

            // Calculate the position based on the ratio
            float mini_lava_position = meter_bar_bottom + (lava_ratio * lava_indicator_range);

            // Set the position
            mini_lava_block_transform.position.y = mini_lava_position;
            mini_lava_loc_transform.position.y = mini_lava_position;
            //std::cout << "mini_lava_position: " << mini_lava_position << "\n";
            const float MAX_GAUGE_HEIGHT = 90.0f; // Adjust this to your desired max height
            float gauge_height = default_lava_gauge + (lava_ratio * (lava_indicator_range - default_lava_gauge));

            float gauge_position = meter_bar_bottom + (gauge_height / 2.0f) + OFFSET_METER_BAR_LAVA;

            lava_gauge_transform.scale.y = gauge_height;
            lava_gauge_transform.position.y = gauge_position;
        }

        //================PLAYER================//
       // std::cout << "======PLAYER data======" << "\n";
        float player_pos_y = player_transform.position.y;
        //std::cout << "player_pos_y: " << player_pos_y << "\n";

        static float default_player_loc_pos = player_loc_transform.position.y; //for testing
        static float default_player_guege_height = player_gauge_transform.scale.y;
        static float default_player_gauge_pos = player_gauge_transform.position.y;
        static float OFFSET_METER_BAR = 100.0f; 
        static float OFFSET_GAUGE = 4.0f;

#if 1
        if (player_pos_y <= START_Y && player_pos_y >= map_bottom_y)
        {
            
            float player_depth_ratio = (START_Y - player_pos_y) / total_map_height; 
            float player_mini_map_y = default_player_loc_pos - (player_depth_ratio *(meter_bar_init_height - default_player_guege_height - OFFSET_METER_BAR));
           
            player_loc_transform.position.y = player_mini_map_y;
            mini_player_icon_transform.position.y = player_loc_transform.position.y;

            static float initial_gauge_height = player_gauge_transform.scale.y;
           
            float gauge_height = initial_gauge_height + (player_depth_ratio * (meter_bar_init_height - default_player_guege_height - OFFSET_METER_BAR));
            float gauge_position = (meter_bar_top - OFFSET_GAUGE) - (gauge_height / 2.0f);

            player_gauge_transform.scale.y = gauge_height;
            player_gauge_transform.position.y = gauge_position;

        }
        else //reset if player stand on map 
        {
            player_gauge_transform.scale.y = default_player_guege_height;
            player_gauge_transform.position.y = default_player_gauge_pos;
            player_loc_transform.position.y = default_player_loc_pos;
            mini_player_icon_transform.position.y = default_player_loc_pos;

        }

        //std::cout << "\n\n";
#endif
    }

}// end of lof 



#if 0
        //float lava_pool_interact = lava_pool_y + (lava_pool_transform.scale.y / 2);

if (lava_pool_interact >= map_bottom_y) // -9786 >= -7734 starting
{
#if 1
    //std::cout << "yyes!!!!\n";
    float lava_rise_amount = lava_pool_interact - map_bottom_y; //keep changeeeeeeee

    float total_possible_rise = (map_bottom_y + (level_rows * tiles_size)) - initial_lava_y; //10176

    //float total_map_height = level_rows * tiles_size; // 7584 
    //std::cout << "total_map_height : " << total_map_height << "\n\n";
    float lava_rise_ratio = std::max(0.0f, std::min(lava_rise_amount / total_possible_rise, 1.0f)); //0.12

    float mini_lava_position = meter_bar_bottom + (lava_rise_ratio * meter_bar_init_height) + OFFSET_METER_BAR_LAVA;


    mini_lava_block_transform.position.y = mini_lava_position;
    mini_lava_loc_transform.position.y = mini_lava_block_transform.position.y;
    //std::cout << "lava_rise_ratio: " << lava_rise_ratio << "\n";
    //std::cout << "mini_lava_position: " << mini_lava_position << "\n";

    // Scale lava gauge to reflect rising lava
    float gauge_height = default_lava_gauge + (lava_rise_ratio * (meter_bar_init_height - default_lava_gauge));
    float gauge_position = meter_bar_bottom + (gauge_height / 2.0f) + OFFSET_METER_BAR_LAVA; // Ensure gauge grows from bottom

    lava_gauge_transform.scale.y = gauge_height;
    lava_gauge_transform.position.y = gauge_position;
#endif
}
else
{
    float pre_map_rise = lava_pool_interact - initial_lava_y;
    const float PRE_MAP_MAX_GAUGE_HEIGHT = BELOW_METER_OFFSET * 1.05f;
    //std::cout << "pre_map_rise: " << pre_map_rise << "\n";
    float total_pre_map_distance = map_bottom_y - initial_lava_y; // Total distance from initial position to map bottom (27 rows)
    //std::cout << "total_pre_map_distance: " << total_pre_map_distance << "\n";
    // Calculate ratio of how far lava has risen through the 27 rows
    float pre_map_ratio = std::max(0.0f, std::min(pre_map_rise / total_pre_map_distance, 1.0f));

    // Calculate position to move from default position to meter_bar_bottom
    //float mini_lava_position = default_lava_icon_pos + (pre_map_ratio * BELOW_METER_OFFSET);
    float mini_lava_position = default_lava_icon_pos + (pre_map_ratio * BELOW_METER_OFFSET);
    mini_lava_block_transform.position.y = mini_lava_position;
    mini_lava_loc_transform.position.y = mini_lava_block_transform.position.y;
    //std::cout << "mini_lava_position: " << mini_lava_position << "\n";


    // This will make the gauge grow more slowly at first
    float gauge_height = default_lava_gauge + (pre_map_ratio * (PRE_MAP_MAX_GAUGE_HEIGHT - default_lava_gauge));


    float gauge_position = meter_bar_bottom - PRE_MAP_MAX_GAUGE_HEIGHT + (gauge_height / 2.0f);
    //std::cout << "gauge_position: " << gauge_position << "\n";

    lava_gauge_transform.scale.y = gauge_height; //-597.642
    lava_gauge_transform.position.y = gauge_position;
}

#endif

#if 0

float lava_depth_ratio = (START_Y - lava_pool_interact) / total_map_height;
lava_depth_ratio = std::clamp(lava_depth_ratio, 0.0f, 1.0f);

// Calculate positions using same formula as player
float lava_mini_map_y = meter_bar_top - OFFSET_GAUGE -
(lava_depth_ratio * (meter_bar_init_height - default_lava_gauge - OFFSET_METER_BAR));

mini_lava_block_transform.position.y = lava_mini_map_y;
mini_lava_loc_transform.position.y = lava_mini_map_y;

// Calculate gauge height to match player's gauge calculation
float gauge_height = default_lava_gauge +
(lava_depth_ratio * (meter_bar_init_height - default_lava_gauge - OFFSET_METER_BAR));

float gauge_position = (meter_bar_top - OFFSET_GAUGE) - (gauge_height / 2.0f);

lava_gauge_transform.scale.y = gauge_height;
lava_gauge_transform.position.y = gauge_position;
#endif

#if 0

float lava_rise_amount = lava_pool_interact - map_bottom_y; //keep changeeeeeeee

float total_possible_rise = (map_bottom_y + (level_rows * tiles_size)) - initial_lava_y; //10176


float lava_rise_ratio = std::max(0.0f, std::min(lava_rise_amount / total_possible_rise, 1.0f)); //0.12
//float mini_lava_position = meter_bar_bottom + (lava_rise_ratio * meter_bar_init_height) + OFFSET_METER_BAR_LAVA + 95.0f; // starting position should be -505uda

//mini_lava_block_transform.position.y = mini_lava_position;
//mini_lava_loc_transform.position.y = mini_lava_position;
//std::cout << "mini_lava_position: " << mini_lava_position << "\n";
//// Scale and position t
//// he lava gauge based on the rise ratio
//float gauge_height = default_lava_gauge + (lava_rise_ratio * (meter_bar_init_height - default_lava_gauge)) + 95.0f;
//float gauge_position = meter_bar_bottom + (gauge_height / 2.0f) + OFFSET_METER_BAR_LAVA;
//lava_gauge_transform.scale.y = gauge_height;
//lava_gauge_transform.position.y = gauge_position;


// Define position range (-505 at bottom to meter_bar_top with small offset)
const float LAVA_START_POSITION = -505.0f;  // Exact starting Y (-505)
const float LAVA_END_POSITION = meter_bar_top - 5.0f;  // Top with small offset
const float LAVA_RANGE = LAVA_END_POSITION - LAVA_START_POSITION;

// 2. Calculate world-space lava progression
float lava_world_start = map_bottom_y;  // When lava touches map bottom
float lava_world_end = START_Y;         // When lava reaches top of map
float lava_world_range = lava_world_end - lava_world_start;

// 3. Current lava position in world space
float lava_current = lava_pool_interact;  // Top of lava pool

// 4. Calculate normalized progress (0-1)
float lava_progress = std::clamp((lava_current - lava_world_start) / lava_world_range, 0.0f, 1.0f);

// 5. Position lava icon (lerp between start/end)
float mini_lava_position = LAVA_START_POSITION + (lava_progress * LAVA_RANGE);
mini_lava_block_transform.position.y = mini_lava_position;
mini_lava_loc_transform.position.y = mini_lava_position;

// 6. Calculate gauge (grows from default to fill remaining space)
float gauge_max_height = LAVA_RANGE;  // Could adjust this if needed
float gauge_height = default_lava_gauge + (lava_progress * gauge_max_height);

// 7. Position gauge (anchored to lava icon position)
float gauge_position = mini_lava_position - (gauge_height / 2.0f);

// Apply transformations
lava_gauge_transform.scale.y = gauge_height;
lava_gauge_transform.position.y = gauge_position;

//// Debug output
//std::cout << "Progress: " << lava_progress << "\n"
//    << "Lava Position: " << mini_lava_position << "\n"
//    << "Gauge Height: " << gauge_height << "\n";

#endif