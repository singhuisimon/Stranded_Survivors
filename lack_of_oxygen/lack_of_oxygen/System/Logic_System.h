// Logic_System.h
#ifndef LOF_LOGIC_SYSTEM_H
#define LOF_LOGIC_SYSTEM_H

#include "System.h"
#include "../Component/Component.h"

namespace lof {

    class Logic_System : public System {
    public:
        Logic_System();
        std::string get_type() const override;
        void update(float delta_time) override;

    private:
        void update_linear_movement(Logic_Component& logic, Transform2D& transform, float delta_time);
        void update_circular_movement(Logic_Component& logic, Transform2D& transform, float delta_time);
    };

} // namespace lof

#endif // LOF_LOGIC_SYSTEM_H