#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/remote_base/remote_base.h"

#include "tac910_protocol.h"

namespace esphome {
namespace ir_tadiran_tac910 {

const float TADIRAN_TAC910_MIN_TEMP = 16.0;
const float TADIRAN_TAC910_MAX_TEMP = 30.0;
const float TADIRAN_TAC910_TARGET_PRECISION = 1.0;

const uint8_t TAC910_FAN_LOW = 0;
const uint8_t TAC910_FAN_MEDIUM = 1;
const uint8_t TAC910_FAN_HIGH = 2;
const uint8_t TAC910_FAN_AUTO = 3;

class TadiranTac910Climate : public climate_ir::ClimateIR {
 public:
  TadiranTac910Climate() : ClimateIR(TADIRAN_TAC910_MIN_TEMP, TADIRAN_TAC910_MAX_TEMP, 
    TADIRAN_TAC910_TARGET_PRECISION, false /* supports_dry */, false /* supports_fan_only */,
    {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
        climate::CLIMATE_FAN_HIGH} /* fan modes*/) {
          this->set_supports_cool(false);
          this->set_supports_heat(false);
        }

 protected:
  void transmit_state() override;
  bool on_receive(remote_base::RemoteReceiveData data) override;

 private:
  bool is_current_temp_only_changed() {
    return this->prev_current_temp != this->current_temperature && 
    this->prev_target_temp == this->target_temperature && 
    this->prev_fan_mode == this->fan_mode && 
    this->prev_mode == this->mode;
  }
  
  void update_prev_state() {
    this->prev_current_temp = this->current_temperature;
    this->prev_target_temp = this->target_temperature;
    this->prev_fan_mode = this->fan_mode.value();
    this->prev_mode = this->mode;
  }

  uint8_t translate_fan_mode(climate::ClimateFanMode fan_mode) {
    switch (fan_mode) {
      case climate::CLIMATE_FAN_LOW:
        return TAC910_FAN_LOW;
      case climate::CLIMATE_FAN_MEDIUM:
        return TAC910_FAN_MEDIUM;
      case climate::CLIMATE_FAN_HIGH:
        return TAC910_FAN_HIGH;
      case climate::CLIMATE_FAN_AUTO:
        return TAC910_FAN_AUTO;
      default:
        return TAC910_FAN_AUTO;
    }
  }

  uint8_t prev_current_temp;
  uint8_t prev_target_temp;
  climate::ClimateFanMode prev_fan_mode;
  climate::ClimateMode prev_mode;
};


}  // namespace ir_tadiran_tac910
}  // namespace esphome
