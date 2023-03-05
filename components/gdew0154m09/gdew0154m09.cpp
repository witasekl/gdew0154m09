#include "gdew0154m09.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace gdew0154m09 {

static const char *const TAG = "gdew0154m09";

void GDEW0154M09::setup() {
  setup_pins_();
  initialize();
  init_internal_(get_buffer_length_());
  clear_screen_();
}

void GDEW0154M09::dump_config() {
  LOG_DISPLAY("", "GDEW0154M09", this);
  LOG_PIN("  CS Pin: ", cs_);
  LOG_PIN("  DC Pin: ", dc_pin_);
  LOG_PIN("  Busy Pin: ", busy_pin_);
  LOG_PIN("  Reset Pin: ", reset_pin_);
  LOG_UPDATE_INTERVAL(this);
}

void GDEW0154M09::update() {
  do_update_();
  if (is_image_changed_()) {
    display_();
  }
}

void GDEW0154M09::fill(Color color) {
  const uint32_t buffer_half = get_buffer_length_() / 2u;
  const uint8_t fill = color.is_on() ? 0x00 : 0xFF;

  for (uint32_t i = 0; i < buffer_half; i++)
    buffer_[i] = fill;
}

void GDEW0154M09::initialize() {
  delay(1000);
  command(0x00);  // Panel setting
  data(0xdf);     // 11 -> 200x200; 0 -> LUT from OTP; 1 -> BW mode;
                  // 1 -> US scan up; 1 -> SHL shift right; 1 -> SHD_N booster on; 1 -> RST_N no effect
  data(0x0e);     // 000; 0 -> VCOM no effect;
                  // 1 -> before enabling booster, temp sensing on; 1 -> VGL will be tied to GND;
                  // 1 -> expect refreshing display, VCOM is tied to GND; 0 -> display off, VCON is set to floating
  command(0x4D);  // FIT iternal code
  data(0x55);
  command(0xaa);
  data(0x0f);
  command(0xe9);
  data(0x02);
  command(0xb6);
  data(0x11);
  command(0xf3);
  data(0x0a);
  command(0x61);  // resolution setting
  data(0xc8);     // 11001000 -> horizontal resolution 200
  data(0x00);     // 0000000; 0
  data(0xc8);     // 11001000 -> vertical resolution 200
  command(0x60);  // TCON setting
  data(0x00);     // 0000 -> source to gate non overlap period 4; 0000 -> gate to source non overlap period 4
  command(0x50);  // VCOM and data interval setting
  data(0xd7);
  command(0x30);  // PLL control
  data(0x3f);     // 00; 111111 -> 29Hz
  command(0xe3);  // Power saving
  data(0x00);
  command(0x04);  // Power on
}

void GDEW0154M09::command(uint8_t value) {
  start_command_();
  write_byte(value);
  end_command_();
}

void GDEW0154M09::data(uint8_t value) {
  start_data_();
  write_byte(value);
  end_data_();
}

void HOT GDEW0154M09::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= get_width_internal() || x < 0 || y >= get_height_internal() || y < 0) {
    ESP_LOGW(TAG, "Position out of area: %dx%d", x, y);
    return;
  }

  int index = get_width_internal() * y + x;

  if (color.is_on()) {
    buffer_[index / 8] &= ~(0x80 >> (index % 8));
  } else {
    buffer_[index / 8] |= (0x80 >> (index % 8));
  }
}

void GDEW0154M09::setup_pins_() {
  dc_pin_->setup();
  dc_pin_->digital_write(true);
  busy_pin_->setup();
  reset_pin_->setup();
  reset_pin_->digital_write(true);
  spi_setup();
  reset_();
}

void GDEW0154M09::reset_() {
  reset_pin_->digital_write(true);
  delay(10);
  reset_pin_->digital_write(false);
  delay(100);
  reset_pin_->digital_write(true);
  delay(100);
}

bool GDEW0154M09::wait_until_idle_() {
  const uint32_t start = millis();
  while (busy_pin_->digital_read()) {
    if (millis() - start > idle_timeout_()) {
      ESP_LOGE(TAG, "Timeout while displaying image!");
      return false;
    }
    delay(10);
  }
  return true;
}

void GDEW0154M09::start_command_() {
  dc_pin_->digital_write(false);
  enable();
}

void GDEW0154M09::end_command_() { disable(); }

void GDEW0154M09::start_data_() {
  dc_pin_->digital_write(true);
  enable();
}

void GDEW0154M09::end_data_() { disable(); }

uint32_t GDEW0154M09::get_buffer_length_() { return get_width_internal() * get_height_internal() / 4u; }

bool GDEW0154M09::is_image_changed_() {
  const uint32_t buffer_half = get_buffer_length_() / 2u;

  for (uint32_t i = 0; i < buffer_half; i++) {
    if (buffer_[i] != buffer_[i + buffer_half]) {
      return true;
    }
  }

  return false;
}

void GDEW0154M09::display_() {
  const uint32_t buffer_half = get_buffer_length_() / 2u;

  command(0x10);
  for (uint32_t i = buffer_half; i < buffer_half * 2u; i++) {
    data(buffer_[i]);
  }
  delay(2);
  command(0x13);
  for (uint32_t i = 0; i < buffer_half; i++) {
    buffer_[i + buffer_half] = buffer_[i];
    data(buffer_[i]);
  }
  delay(2);
  command(0x12);
  wait_until_idle_();
}

void GDEW0154M09::clear_screen_() {
  const uint32_t buffer_half = get_buffer_length_() / 2u;

  delay(1000);

  command(0x10);
  for (uint32_t i = 0; i < buffer_half; i++) {
    data(0xff);
  }
  delay(2);
  command(0x13);
  for (uint32_t i = 0; i < buffer_half; i++) {
    data(0x00);
  }
  delay(2);
  command(0x12);
  wait_until_idle_();

  command(0x10);
  for (uint32_t i = 0; i < buffer_half; i++) {
    data(0x00);
  }
  delay(2);
  command(0x13);
  for (uint32_t i = 0; i < buffer_half; i++) {
    data(0xff);
    buffer_[i] = buffer_[i + buffer_half] = 0xff;
  }
  delay(2);
  command(0x12);
  wait_until_idle_();
}

}  // namespace gdew0154m09
}  // namespace esphome
