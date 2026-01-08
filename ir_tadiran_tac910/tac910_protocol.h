#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace remote_base {

struct TacDataCmd {
    // byte 1
    uint8_t current_temp; // +5 for real value

    //byte 2
    uint8_t set_temp; // +5 for real value

    // byte 3
    uint8_t u3_0:4;
    uint8_t speed:2; // 0 low, 1 mid, 2 high, 3 auto
    uint8_t state:2; // 0 off, 1 on
    uint8_t u4; // seq_num
    uint8_t u5; // 0xff
    uint8_t u6; // 0xff
    uint8_t u7; // checksum 0x4a or 0x42
};

struct TacDataUpdateTemp {
    // 15 bits only
    uint8_t current_temp; // +5 for real value
    uint8_t suffix; // const 0x46
};

struct TacData {
    constexpr static uint8_t CMD_BITS = 56;
    constexpr static uint8_t UPDATE_TEMP_BITS = 15;
    union{
        uint8_t data[7];
        TacDataCmd cmd;
        TacDataUpdateTemp update;
    };
    uint8_t len_bits;

    void set_current_temp(uint8_t temp) { this->cmd.current_temp = temp - 5; }
    void set_desired_temp(uint8_t temp) { this->cmd.set_temp = temp - 5; }
    uint8_t get_checksum_for_cmd();
    void prepare();
};

class TacProtocol : public RemoteProtocol<TacData> {
 public:
  void encode(RemoteTransmitData *dst, const TacData &data) override;
  void encode_frame(RemoteTransmitData *dst, const TacData &data);

  optional<TacData> decode(RemoteReceiveData src) override;
  void dump(const TacData &data) override;

 private:
  void dump_(const TacData &data, const char *prefix);
};

DECLARE_REMOTE_PROTOCOL(Tac)

template<typename... Ts> class TacAction: public RemoteTransmitterActionBase<Ts...>{
    public:
    TEMPLATABLE_VALUE(uint8_t, nbits)
    TEMPLATABLE_VALUE(uint8_t, current_temp)
    TEMPLATABLE_VALUE(uint8_t, desired_temp)
    TEMPLATABLE_VALUE(uint8_t, speed)
    TEMPLATABLE_VALUE(uint8_t, state)

    void encode(RemoteTransmitData *dst, Ts... x) override{
        TacData data{};
        data.len_bits = this->nbits_.value(x...);
        data.set_current_temp(this->current_temp_.value(x...));
        data.set_desired_temp(this->desired_temp_.value(x...));
        data.cmd.speed = this->speed_.value(x...);
        data.cmd.state = this->state_.value(x...);

        TacProtocol().encode(dst, data);
    }
};

}  // namespace remote_base
}  // namespace esphome
