#include "esphome/core/log.h"
#include "ir_tadiran_tac910.h"
#include "tac910_protocol.h"

namespace esphome {
namespace ir_tadiran_tac910 {

static const char *TAG = "tac910.climate";

void TadiranTac910Climate::transmit_state() {
    esphome::remote_base::TacData tac_data;
    if (this->is_current_temp_only_changed()) {
        tac_data.set_current_temp(this->current_temperature);
        tac_data.len_bits = esphome::remote_base::TacData::UPDATE_TEMP_BITS;
    } else {
        tac_data.set_current_temp(this->current_temperature);
        tac_data.set_desired_temp(this->target_temperature);
        tac_data.cmd.speed = this->translate_fan_mode(this->fan_mode.value());
        tac_data.cmd.state = (uint8_t)this->mode;
        tac_data.len_bits = esphome::remote_base::TacData::CMD_BITS;
    }

    // transmit
    auto transmit = this->transmitter_->transmit();
    auto *data = transmit.get_data();
    
    esphome::remote_base::TacProtocol().encode(data, tac_data);
    transmit.perform();
    this->update_prev_state();
}

bool TadiranTac910Climate::on_receive(remote_base::RemoteReceiveData data) {
    return false;
}

}  // namespace ir_tadiran_tac910
}  // namespace esphome
