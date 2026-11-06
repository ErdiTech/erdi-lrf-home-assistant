#pragma once

#include <string>

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

#include "erdi_lrf_core.h"

namespace esphome {
namespace erdi_lrf {

class ErdiLRFSensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void set_model(const std::string &model) { model_name_ = model; }

  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  std::string model_name_;
  const erdi_model_t *model_{nullptr};
  erdi_parser_t parser_{};
  bool have_value_{false};
  float latest_{NAN};
  uint8_t missed_updates_{0};
};

}  // namespace erdi_lrf
}  // namespace esphome
