#include "erdi_lrf.h"

#include <cmath>

#include "esphome/core/log.h"

namespace esphome {
namespace erdi_lrf {

static const char *const TAG = "erdi_lrf";

void ErdiLRFSensor::setup() {
  this->model_ = erdi_find_model(this->model_name_.c_str());
  if (this->model_ == nullptr) {
    ESP_LOGE(TAG, "Unknown ERDI model %s", this->model_name_.c_str());
    this->mark_failed();
    return;
  }
  erdi_parser_init(&this->parser_, this->model_);
}

void ErdiLRFSensor::loop() {
  if (this->model_ == nullptr) return;
  uint8_t byte;
  erdi_measurement_t m;
  while (this->available() && this->read_byte(&byte)) {
    if (erdi_parser_feed(&this->parser_, byte, &m)) {
      this->latest_ = m.out_of_range ? NAN : m.metres;
      this->have_value_ = true;
      this->missed_updates_ = 0;
    }
  }
}

void ErdiLRFSensor::update() {
  if (this->model_ == nullptr) return;
  if (this->model_->family == ERDI_FAMILY_A) {
    // Family A is polled: ask for one single-shot measurement; the reply is parsed in loop().
    // Publish whatever the previous request produced.
    uint8_t req[8];
    erdi_single_shot_request(req);
    this->write_array(req, sizeof(req));
  }
  if (this->have_value_) {
    this->publish_state(this->latest_);
    this->have_value_ = false;
  } else if (++this->missed_updates_ >= 3) {
    // No valid frame for three update intervals: report unknown instead of a stale distance.
    this->publish_state(NAN);
    this->missed_updates_ = 3;
  }
}

void ErdiLRFSensor::dump_config() {
  LOG_SENSOR("", "ERDI laser rangefinder", this);
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_name_.c_str());
  if (this->model_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Protocol family: %s, expected baud rate: %u",
                  this->model_->family == ERDI_FAMILY_A ? "A (55 AA, polled)" : "B (5C, stream)",
                  (unsigned) this->model_->baud);
  }
  LOG_UPDATE_INTERVAL(this);
  this->check_uart_settings(this->model_ != nullptr ? this->model_->baud : 115200);
}

}  // namespace erdi_lrf
}  // namespace esphome
