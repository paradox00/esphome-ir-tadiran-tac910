#include "tac910_protocol.h"
#include "esphome/core/log.h"

#include "esphome/components/remote_base/pronto_protocol.h"
#include "esphome/components/remote_base/raw_protocol.h"

namespace esphome {
namespace remote_base {

static const char *const Tac_TAG = "remote.tac910";

static const uint16_t TAC_FREQ = 38000;

static const uint16_t TAC_HEADER_MARK = 5900;
static const uint16_t TAC_HEADER_SPACE = 1800;

static const uint16_t TAC_FOOTER_MARK = 7800;
static const uint16_t TAC_FOOTER_SPACE = 500;

static const uint16_t TAC_MARK = 580;
static const uint16_t TAC_ZERO = 500;
static const uint16_t TAC_ONE = 1600;

uint8_t TacData::get_checksum_for_cmd()
{
    uint32_t total = 0;
    for (size_t i = 0; i < sizeof(cmd) - 1; i++) {
        uint8_t byte = data[i];
        while (byte) {
            total += byte & 1;   // add lowest bit
            byte >>= 1;          // shift right
        }
    }
    // If ODD -> use 0x42, else 0x4A
    return (total % 2) ? 0x4A : 0x42;
}

void TacData::prepare()
{
    if (this->len_bits == TacData::UPDATE_TEMP_BITS){
        this->update.suffix = 0x46;
    } else if (this->len_bits == TacData::CMD_BITS) {
        this->cmd.u3_0 = 0x5;
        this->cmd.u4 = (uint8_t) micros() & 0xff;
        this->cmd.u5 = 0xff;
        this->cmd.u6 = 0xff;
        this->cmd.u7 = get_checksum_for_cmd();
    }
}

void TacProtocol::encode(RemoteTransmitData *dst, const TacData &data) 
{
    TacData full_data = data;
    full_data.prepare();

    dst->set_carrier_frequency(TAC_FREQ);
    dst->reserve(3 /*frames*/ * (2 /*header*/ + 2 * data.len_bits) + 2 /*footer*/);
    for (int i = 0; i < 3; i++){
        encode_frame(dst, full_data);
    }

    // footer
    dst->item(TAC_FOOTER_MARK, TAC_FOOTER_SPACE);

    dump_(full_data, "Sending");
#if DEBUG
    ESP_LOGI(Tac_TAG, "total length %d", dst->get_data().size());

    RemoteReceiveData dump_data(dst->get_data(), 30, ToleranceMode::TOLERANCE_MODE_PERCENTAGE);
    ESP_LOGI(Tac_TAG, "sending Tac: %02X %02X %02X %02X %02X %02X %02X (%d bits)", full_data.data[0], full_data.data[1], full_data.data[2], full_data.data[3], full_data.data[4], full_data.data[5], full_data.data[6], full_data.len_bits);
    auto pronto = ProntoProtocol().decode(dump_data);
    ProntoProtocol().dump(pronto.value());
    ESP_LOGI(Tac_TAG, "finish dumping pronto");
    RawDumper().dump(dump_data);
    ESP_LOGI(Tac_TAG, "finish dumping raw");
#endif
}

void TacProtocol::encode_frame(RemoteTransmitData *dst, const TacData &data)
{
    dst->item(TAC_HEADER_MARK, TAC_HEADER_SPACE);
    for (int i = 0; i < data.len_bits / 8; i++){
        for (uint8_t mask = 1<<7; mask; mask >>= 1) {
            dst->item(TAC_MARK, (data.data[i] & mask) ? TAC_ONE : TAC_ZERO);
        }
    }
}

optional<TacData> TacProtocol::decode(RemoteReceiveData src) 
{
    TacData data{};

    // Check if header matches
    if (!src.expect_item(TAC_HEADER_MARK, TAC_HEADER_SPACE)) {
        return {};
    }

    ESP_LOGI(Tac_TAG, "detected Tac protocol");

    for (int i = 0; i < sizeof(data.data); i++) {
        // Read bit
        for (int j = 7; j >= 0; j--) {
            if (src.expect_item(TAC_MARK, TAC_ONE)) {
                data.data[i] |= 1 << j;

            } else if (!src.expect_item(TAC_MARK, TAC_ZERO)) {
                ESP_LOGV(Tac_TAG, "Byte %d bit %d fail", i, j);
                if (data.len_bits < 7) {
                    ESP_LOGV("don't report less then 3 bits as protocol detected");
                    return {};
                }
                return data;
            }
            ++data.len_bits;
        }

        ESP_LOGVV(Tac_TAG, "Byte %d %02X", i, data.data[i]);
    }

    //dump(data);
    //return {};
    return data;
}

void TacProtocol::dump_(const TacData &data, const char *prefix) 
{
  ESP_LOGI(Tac_TAG, "%s Tac: %02X %02X %02X %02X %02X %02X %02X (%d bits)", prefix, data.data[0], data.data[1], data.data[2], data.data[3], data.data[4], data.data[5], data.data[6], data.len_bits);
  if (data.len_bits == 56){
    ESP_LOGI(Tac_TAG, "parsed: curr temp: %d, set temp: %d, state: %d, speed: %d", 
             data.cmd.current_temp + 5, data.cmd.set_temp + 5, data.cmd.state, data.cmd.speed);
  } else if (data.len_bits == 15) {
      ESP_LOGI(Tac_TAG, "parsed: curr temp: %d, suffix: 0x%x", data.update.current_temp + 5, data.update.suffix);
  }
}

void TacProtocol::dump(const TacData &data)
{
    dump_(data, "Received");
}

}  // namespace remote_base
}  // namespace esphome
