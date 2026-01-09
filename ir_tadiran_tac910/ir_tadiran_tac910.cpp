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
  auto decoded_opt = esphome::remote_base::TacProtocol().decode(data);
  if (!decoded_opt.has_value())
    return false;
  auto decoded = decoded_opt.value();

  if (decoded.len_bits == esphome::remote_base::TacData::UPDATE_TEMP_BITS) {
    this->current_temperature = decoded.get_current_temp();
  } else if (decoded.len_bits == esphome::remote_base::TacData::CMD_BITS) {
    this->current_temperature = decoded.get_current_temp();
    this->target_temperature = decoded.get_desired_temp();

    if (decoded.cmd.state == 0) {
      this->mode = climate::CLIMATE_MODE_OFF;
    } else if (decoded.cmd.state == 1) {
      this->mode = climate::CLIMATE_MODE_HEAT_COOL;
    }

    switch (decoded.cmd.speed) {
      case TAC910_FAN_LOW:
        this->fan_mode = climate::CLIMATE_FAN_LOW;
        break;
      case TAC910_FAN_MEDIUM:
        this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
        break;
      case TAC910_FAN_HIGH:
        this->fan_mode = climate::CLIMATE_FAN_HIGH;
        break;
      case TAC910_FAN_AUTO:
        this->fan_mode = climate::CLIMATE_FAN_AUTO;
        break;
    }
  } else {
    return false;
  }

  this->publish_state();
  return true;
}

}  // namespace ir_tadiran_tac910
}  // namespace esphome
