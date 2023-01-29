#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace gdew0154m09 {

class GDEW0154M09;

using gdew0154m09_writer_t = std::function<void(GDEW0154M09 &)>;

class GDEW0154M09 : public PollingComponent,
                    public display::DisplayBuffer,
                    public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_HIGH, spi::CLOCK_PHASE_TRAILING,
                                          spi::DATA_RATE_10MHZ> {
 public:
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_busy_pin(GPIOPin *pin) { busy_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  void update() override;
  void fill(Color color) override;

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

  void initialize();
  void command(uint8_t value);
  void data(uint8_t value);

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_height_internal() override { return 200; }
  int get_width_internal() override { return 200; }

  void setup_pins_();
  void reset_();
  bool wait_until_idle_();
  uint32_t idle_timeout_() { return 1000u; }
  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();
  uint32_t get_buffer_length_();
  void clear_buffer_();
  bool is_image_changed_();
  void display_();

  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_;
  GPIOPin *reset_pin_;
};

}  // namespace gdew0154m09
}  // namespace esphome
